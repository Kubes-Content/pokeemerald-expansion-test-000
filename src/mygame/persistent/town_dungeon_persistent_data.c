//
// Created by kubes on 9/21/26.
//
#include "mygame/persistent/town_dungeon_persistent_data.h"

#include "global.h"

#include "overworld.h"
#include "palette.h"
#include "random.h"
#include "constants/event_objects.h"
#include "mygame/dungeon_generation/dungeon_generation_global.h"
#include "mygame/patches/dynamic_warp/town_dungeon_warps.h"


struct TownDungeonGamePersistentData* GetTownDungeonGamePersistentData()
{
    return &gSaveBlock1Ptr->townDungeonData;
}

struct TownDungeonPersistentData* GetCurrentTownDungeonData()
{
    struct TownDungeonGamePersistentData* this = GetTownDungeonGamePersistentData();
    return &this->dungeonTownData[this->currentTown];
}

static struct CellVariant GetEntryCaveMap()
{
    const struct CellVariant townStandInCell = CellVariant_Create(MAP_CAVE_TOWN_00,1,0,0,0);
    //
    return GetConnectableCell(&townStandInCell, RelativeCellConnection_Create(CONNECTION_NORTH));
}

static enum Connection Reverse(const enum Connection direction)
{
    fatal_assertf(direction >= CONNECTION_SOUTH);
    fatal_assertf(direction <= CONNECTION_EAST);

    switch (direction)
    {
    case CONNECTION_SOUTH:
        return CONNECTION_NORTH;
    case CONNECTION_NORTH:
        return CONNECTION_SOUTH;
    case CONNECTION_WEST:
        return CONNECTION_EAST;
    case CONNECTION_EAST:
        return CONNECTION_WEST;
    default:
        fatalf("invalid connection enum value");
    }
}

// TODO this sucks, get rid of it
static u8 GetPlaceholderCellWarpIdForDirection(const enum Connection direction)
{
    switch (direction)
    {
    case CONNECTION_SOUTH:
        return 0;
    case CONNECTION_NORTH:
        return 1;
    case CONNECTION_WEST:
        return 2;
    case CONNECTION_EAST:
        return 3;
    default:
        fatalf("bad connection enum");
    }
}

// TODO do this intelligently // not just cardinal directions
// returns number of elements written to array
static u8 GetAllRelativeCellConnections(const struct CellVariant* cellVariant, struct RelativeCellConnection* out_connections, const u8 connectionsArrayCapacity)
{
    fatal_assertf(connectionsArrayCapacity >= 4);
    u8 count = 0;

    for (; count < 4; count++)
    {
        const enum Connection direction = count + CONNECTION_SOUTH;
        out_connections[count] = RelativeCellConnection_Create(direction);
    }

    return count;
}

static struct RelativeCellConnection PickAnyOtherCellMapRelativeConnection(const struct CellVariant* cellVariant, const struct RelativeCellConnection* unwantedConnectionsArr, const u8 unwantedConnectionsCount)
{
    const u8 connectionsCapacity = 4;
    struct RelativeCellConnection connections[connectionsCapacity];
    const u8 connectionsFound = GetAllRelativeCellConnections(cellVariant, connections, connectionsCapacity);

    const u8 chosenIndex = Random() % (connectionsFound - unwantedConnectionsCount); // assuming that every connection is unique and none are like special clones that go to one location ... or something ...
    u8 chosenCount = 0;
    for (u8 i = 0; i < connectionsFound; i++)
    {
        for (u8 unwantedIndex = 0; unwantedIndex < unwantedConnectionsCount; unwantedIndex++)
        {
            if (RelativeCellConnection_Equal(&unwantedConnectionsArr[unwantedIndex], &connections[i]))
                goto continue_connection;
        }

        if (chosenCount == chosenIndex)
        {
            return connections[i];
        }

        chosenCount++;

continue_connection:
        continue;
    }
    fatalf("No result found for PickAnyOtherCellMapRelativeConnection"); // TODO useful message
}

// TODO extract
// usedCoordinates2dArr's size is (layout->width * layout->height)
static bool8 IsTilePassable(const struct MapLayout* layout, const u8 x, const u8 y, const bool8* usedCoordinates2dArr)
{
    const bool8 containsAGeneratedCollidableObject = usedCoordinates2dArr[x + y * layout->width];
    const bool8 passableTile = UNPACK_COLLISION(layout->map[x + y * layout->width]) == COLLISION_NONE;
    return !containsAGeneratedCollidableObject && passableTile;
}

static void FindOpenCoordinate(const struct MapLayout* layout, const bool8* usedCoordinates2dArr, u8* xPtr, u8* yPtr)
{
    const u8 maxAttempts = 29;
    for (u8 attempt = 0; attempt < maxAttempts; attempt++)
    {
        *xPtr = Random() % layout->width;
        *yPtr = Random() % layout->height;

        const bool8 alreadyChosenLocation = usedCoordinates2dArr[*xPtr + *yPtr * layout->width];
        if (alreadyChosenLocation) continue;

        if (!IsTilePassable(layout, *xPtr, *yPtr, usedCoordinates2dArr)) continue;

        goto chosen;
    }
    fatalf();
    chosen:;
}

// reduce validSharedPickupDescriptions from NUM_PICKUP_DESCRIPTIONS_PER_DUNGEON to not pick from the full description array
    // todo a nicer way to pick a range across the descriptions array, like index 2 to index 6, or even wrapping around the array
static void CreatePickupPermanentCellData(const struct MapLayout* layout, struct CellPickupConfig* pickupConfigsArr, const u8 pickupConfigsArrCapacity,
                                            const u8 pickupConfigsArrCount, const u8 validSharedPickupDescriptions, bool8* usedCoordinates2dArr)
{
    fatal_assertf(pickupConfigsArrCapacity >= pickupConfigsArrCount);

    for (u8 i = 0; i < pickupConfigsArrCount; i++)
    {
        u8 x, y;
        FindOpenCoordinate(layout, usedCoordinates2dArr, &x, &y);

        pickupConfigsArr[i] = CellPickupConfig_Create(x, y, Random() % validSharedPickupDescriptions);

        usedCoordinates2dArr[x + y * layout->width] = TRUE;
    }
}

// TODO we need a temporary bool mask of the map - to determine if we have already chosen to place an object at a coordinate
    // a bool8 2D array
    // I don't think we even need to pass the size since it'll be based on the target map
static void CreateMonsterPermanentCellData(const struct MapLayout* layout, struct CellOverworldMonsterConfig* cellMonsterConfigsArr, const u8 cellMonsterConfigsArrCapacity,
                                            const u8 cellMonsterConfigsArrCount, const u8 validSharedMonsterDescriptions, bool8* usedCoordinates2dArr)
{
    fatal_assertf(cellMonsterConfigsArrCapacity >= cellMonsterConfigsArrCount);

    // TODO 2d bool arr for already chosen coordinates

    for (u8 i = 0; i < cellMonsterConfigsArrCount; i++)
    {
        u8 x, y;
        FindOpenCoordinate(layout, usedCoordinates2dArr, &x, &y);

        cellMonsterConfigsArr[i] = CellOverworldMonsterConfig_Create(x, y, Random() % validSharedMonsterDescriptions);
        usedCoordinates2dArr[x + y * layout->width] = TRUE;
    }
}

static void GenerateCellData(struct CaveData* caveData, const struct CellVariant* cellVariant, const u8 cellIndex, const s8 cellPickupDefinitionCount, const s8 cellMonsterDefinitionCount)
{
    const struct MapHeader* header = Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(cellVariant->mapEnum), MAP_NUM(cellVariant->mapEnum));
    const struct MapLayout* layout = GetMapLayout(header->mapLayoutId);

    // bool map of where we've already spawned things // todo bitmask
    bool8 usedCoordinates2dArr[layout->width * layout->height];

    // TODO randomize pickup locations
    const u8 pickupConfigsArrSize = MAX_PICKUPS_PER_CELL;
    struct CellPickupConfig pickupConfigsArr[pickupConfigsArrSize];
    CreatePickupPermanentCellData(layout, pickupConfigsArr, pickupConfigsArrSize, cellPickupDefinitionCount, NUM_PICKUP_DESCRIPTIONS_PER_DUNGEON, usedCoordinates2dArr);

    const u8 xSize = MAX_OVERWORLD_MONSTERS_PER_CELL;
    struct CellOverworldMonsterConfig monsterConfigsArr[xSize];
    CreateMonsterPermanentCellData(layout, monsterConfigsArr, xSize, cellMonsterDefinitionCount, NUM_OVERWORLD_MONSTER_DESCRIPTIONS_PER_DUNGEON, usedCoordinates2dArr);

    // TODO support entering cave from any direction, NOT TIED TO CARDINAL DIRECTION, abstracted to wrap cardinal direction until we implement a better solution in its place

    caveData->cellsData[cellIndex] = DungeonCellData_Create(GetCellVariantMapEnum(cellVariant), cellPickupDefinitionCount, cellMonsterDefinitionCount, pickupConfigsArr, monsterConfigsArr); // todo reimplement, generate all cells
}

// todo fix: this assumes that entryCell is a room with a single pre-established connection to another cell
// unwantedConnections is relative to entry/owning cell
// TODO this is a disgusting number of arguments; Why is this happening?
static void GenerateCavePathSegment(struct TownDungeonPersistentData* townData, u8* const generatedCellCount, struct CellVariant caveCellVariants[MAX_DUNGEON_CELL_COUNT], const u8 segmentCellCount, const struct CellVariant* entryCell, const u8 entryCellIndex, struct RelativeCellConnection* unwantedConnections, u8 unwantedConnectionsCount, const s8 cellPickupDefinitionCount, const s8 cellMonsterDefinitionCount) // NOLINT(*-non-const-parameter)
{
    struct CaveData* caveData = &townData->caveData;

    const struct CellVariant* previousCell = entryCell;
    u8 previousCellIndex = entryCellIndex;

    const u8 iteratorOffset = *generatedCellCount;
    for (u8 i = iteratorOffset; i < segmentCellCount + iteratorOffset; i++)
    {
        fatal_assertf(*generatedCellCount < MAX_DUNGEON_CELL_COUNT, "Generating %d with a max of %d", *generatedCellCount + 1, (u8) MAX_DUNGEON_CELL_COUNT);
        const u8 newCellIndex = *generatedCellCount;
        struct DungeonCellData* previousCellData = &caveData->cellsData[previousCellIndex];
        struct DungeonCellData* newCellData = &caveData->cellsData[newCellIndex];

        // randomly pick an unused connection in previousCell

        const struct RelativeCellConnection connectionFromPreviousCellToNewCell = PickAnyOtherCellMapRelativeConnection(previousCell, unwantedConnections, unwantedConnectionsCount);

        // todo extract this
        const struct RelativeCellConnection connectionFromNewCellToPreviousCell = RelativeCellConnection_Create(Reverse(connectionFromPreviousCellToNewCell.direction)); // reciprocal = toFrom

        caveCellVariants[newCellIndex] = GetConnectableCell(previousCell, connectionFromPreviousCellToNewCell);
        const struct CellVariant* newCell = &caveCellVariants[newCellIndex]; // see: GenerateSingleCellStep

        // TODO simplify

        GenerateCellData(caveData, newCell, newCellIndex, cellPickupDefinitionCount, cellMonsterDefinitionCount);

        const u8 previousCellFromWarpId = GetPlaceholderCellWarpIdForDirection(connectionFromPreviousCellToNewCell.direction);
        const u8 newCellToWarpId        = GetPlaceholderCellWarpIdForDirection(connectionFromNewCellToPreviousCell.direction);
        //
        previousCellData->connections[previousCellFromWarpId] = DungeonCellConnection_Create(newCellIndex, newCellToWarpId);
        newCellData->connections[newCellToWarpId]             = DungeonCellConnection_Create(previousCellIndex, previousCellFromWarpId);

        MgbaPrintf(MGBA_LOG_WARN, "connected cell %d warp %d TO cell %d warp %d", previousCellIndex, previousCellFromWarpId, newCellIndex, newCellToWarpId);

        previousCell = newCell;
        previousCellIndex = newCellIndex;

        unwantedConnectionsCount = 1;
        unwantedConnections[0] = connectionFromNewCellToPreviousCell;
        (*generatedCellCount)++;
    }
}

// TODO this shouldn't be in this file
// TODO refactor; fix this mess
static void GenerateCaveData(struct TownDungeonPersistentData* this)
{
    struct CaveData* caveData = &this->caveData;
    fatal_assertf(MAX_DUNGEON_CELL_COUNT > caveData->maxIndexForCell, "MAX_DUNGEON_CELL_COUNT too low for generation test");

    struct CellVariant caveCellVariants[MAX_DUNGEON_CELL_COUNT];
    caveCellVariants[0] = GetEntryCaveMap();

    const struct CellVariant* entryCell = &caveCellVariants[0];
    u8 generatedCellCount = 0;

    const s8 caveEntryCellPickupCount = 3; // todo this should be an argument
    const s8 caveEntryCellMonsterCount = 2; // todo this should be an argument
    GenerateCellData(caveData, entryCell, generatedCellCount, caveEntryCellPickupCount, caveEntryCellMonsterCount);
    //
    const s8 townCellIndex = -1; // todo this should probably be a constant of some form
    const u8 townDoorWarpId = 0; // one day this guy will be fetched dynamically. one day.
    const struct DungeonCellConnection connectionFromDungeonToTown = DungeonCellConnection_Create(townCellIndex, townDoorWarpId);
    //
    const u8 cell0ToTownConnectionIndex = 0; // TODO is this because 0 is the south warp id? idk, how would I know?
    caveData->cellsData[generatedCellCount].connections[cell0ToTownConnectionIndex] = connectionFromDungeonToTown; // duplicates enum and warpId used in GetEntryCell

    // wait. is connects' index literally the warp id?
    // shouldn't that be more explicit? like with a function? you monster.

    {
        fatal_assertf(entryCell->hasSouthWarp);
        fatal_assertf(entryCell->hasNorthWarp);
        fatal_assertf(entryCell->hasWestWarp);
        fatal_assertf(entryCell->hasEastWarp);
    }

    const u8 dungeonCellCount = caveData->maxIndexForCell + 1;
    const u8 rootPathMinimumLength = dungeonCellCount / 2 + dungeonCellCount % 2;

    {
        const u8 entryCellIndex = generatedCellCount;
        generatedCellCount++;

        const struct RelativeCellConnection connectionFromPreviousCellToPriorCell = RelativeCellConnection_Create(CONNECTION_SOUTH);

        const u8 unwantedConnectionsCount = 1;
        struct RelativeCellConnection unwantedConnections[unwantedConnectionsCount];
            unwantedConnections[0] = connectionFromPreviousCellToPriorCell;
        const u8 cellsToGenerateForRootPath = rootPathMinimumLength - 1; // first cell already generated

        GenerateCavePathSegment(this, &generatedCellCount, caveCellVariants, cellsToGenerateForRootPath, entryCell, entryCellIndex, unwantedConnections, unwantedConnectionsCount, caveEntryCellPickupCount, caveEntryCellMonsterCount);
    }

    // GEN SUB-PATHS / spawn the rest of the dungeon

    const u8 remainingCellCount = dungeonCellCount - rootPathMinimumLength;

    for (u8 sideRoomIndex = 0; sideRoomIndex < remainingCellCount; sideRoomIndex++)
    {
        const u8 maximumAttempts = 99;
        u8 tryCount = 0;

redo:
        fatal_assertf(tryCount < maximumAttempts);
        tryCount++;

        const u8 ownerCellIndex = Random() % rootPathMinimumLength;
        const struct DungeonCellData* ownerCellData = &caveData->cellsData[ownerCellIndex];
        const struct CellVariant* ownerCellVariant = &caveCellVariants[ownerCellIndex];

        struct RelativeCellConnection unwantedConnectionsArr[MAX_DUNGEON_CELL_CONNECTIONS_PER_CELL];
        u8 unwantedConnectionsCount = 0;
        {
            // TODO find a better way to do this
            u8 unusedConnectionsCount = 0;
            for (u8 i = 0; i < MAX_DUNGEON_CELL_CONNECTIONS_PER_CELL; i++)
            {
                if (ownerCellData->connections[i].isValid)
                {
                    // TODO this is horrendous and I hate you for it
                    unwantedConnectionsArr[unwantedConnectionsCount] = RelativeCellConnection_Create(CONNECTION_SOUTH + i);
                    unwantedConnectionsCount++;
                }
                else unusedConnectionsCount++;
            }

            if (unusedConnectionsCount == 0) goto redo; // find a cell with unused doors
        }

        const u8 subPathLength = 1;
        GenerateCavePathSegment(this, &generatedCellCount, caveCellVariants, subPathLength, ownerCellVariant, ownerCellIndex, unwantedConnectionsArr, unwantedConnectionsCount, caveEntryCellPickupCount, caveEntryCellMonsterCount);
    }

    // debug connections

#ifndef NDEBUG
    u8 invalidCount = 0;
    for (u8 i = 0; i <= caveData->maxIndexForCell; i++)
    {
        const struct DungeonCellData* dungeonCellData = &caveData->cellsData[i];
        const u8 connectionsLength = sizeof(dungeonCellData->connections) / sizeof(dungeonCellData->connections[0]);
        for (u8 connectionIndex = 0; connectionIndex < connectionsLength; connectionIndex++)
        {
            const struct DungeonCellConnection* dungeonCellConnection = &dungeonCellData->connections[connectionIndex];
            if (!dungeonCellConnection->isValid) continue;
            if (dungeonCellConnection->isTown) continue;

            const struct DungeonCellData* otherCellData = &caveData->cellsData[dungeonCellConnection->cellIndex];
            const struct DungeonCellConnection* oppositeCellConnection = &otherCellData->connections[dungeonCellConnection->warpId];

            if (!oppositeCellConnection->isValid)
            {
                fatalf("failed connection cell %d warp %d TO cell %d warp %d", i, connectionIndex, dungeonCellConnection->cellIndex, dungeonCellConnection->warpId);
                invalidCount++;
            }
        }
    }
    fatal_assertf(invalidCount == 0);
#endif
}

static void InitializeTownDungeonConfig(struct TownDungeonPersistentData* this)
{
    this->dummyTownData = TownData_Create();
    this->caveData = CaveData_Create();

    GenerateCaveData(this);
}

void InitializeTownDungeonGameConfig(struct TownDungeonGamePersistentData* this)
{
    this->currentTown = 0;
    this->rootTaskId = NUM_TASKS;
    ClearTemporaryStatePerContext(this);

    // todo random initial set
    for (u8 i = 0; i < NUM_PICKUP_DESCRIPTIONS_PER_DUNGEON; i++)
    {
        struct DummyPickupDescription* thisDescription = &this->sharedCavePickupDescriptions[i];
        const u8 pickupOptions = 3;
        const u8 chosenOptionIndex = i % pickupOptions;

        if (chosenOptionIndex == 0)
            *thisDescription = PickupDescription_Create(ITEM_POTION, OBJ_EVENT_GFX_ITEM_BALL);
        else if (chosenOptionIndex == 1)
            *thisDescription = PickupDescription_Create(ITEM_SUN_STONE, OBJ_EVENT_GFX_BALL_CUSHION);
        else if (chosenOptionIndex == 2)
            *thisDescription = PickupDescription_Create(ITEM_SUPER_REPEL, OBJ_EVENT_GFX_KISS_CUSHION);
        else
            fatalf("pickupOptions' bounds exceeded.");
    }

    const u8 sharedMonsterDescriptionsLength = sizeof(this->sharedMonsterDescriptions) / sizeof(this->sharedMonsterDescriptions[0]);
    for (u8 i = 0; i < sharedMonsterDescriptionsLength; i++)
    {
        const enum Species maxKnownOverworldSpecies = 386;
        this->sharedMonsterDescriptions[i] = OverworldMonsterDescription_Create(Random() % maxKnownOverworldSpecies + 1);
    }

    struct TownDungeonPersistentData* initialTownData = GetCurrentTownDungeonData();
    InitializeTownDungeonConfig(initialTownData);
}

void ClearTemporaryStatePerContext(struct TownDungeonGamePersistentData* this)
{
    memset(&this->temporaryStatePerContext, 0, sizeof(this->temporaryStatePerContext));
}

bool8 RootTaskIsRunning(const struct TownDungeonGamePersistentData* this)
{
    return this->rootTaskId != NUM_TASKS;
}

void KillRootTask(struct TownDungeonGamePersistentData* this)
{
    fatal_assertf(RootTaskIsRunning(this));

    DestroyTask(this->rootTaskId);

    this->rootTaskId = NUM_TASKS;
}

u8 SetRootTask(struct TownDungeonGamePersistentData* this, const TaskFunc func)
{
    fatal_assertf(!RootTaskIsRunning(this));

    const u8 taskId = CreateTask(func, 0);
    fatal_assertf(taskId != NUM_TASKS);

    return this->rootTaskId = taskId;
}

void PreDynamicWarp(u8 enteredWarpId)
{
    struct TownDungeonGamePersistentData* rootGameData = GetTownDungeonGamePersistentData();

    if (RootTaskIsRunning(rootGameData))
        KillRootTask(rootGameData);

    // todo extract this behavior to town_dungeon_persistent_data.c
    if (rootGameData->context == CONTEXT_TOWN)
        SetDynamicWarpFromDungeonTownWarp(enteredWarpId);
    else if (rootGameData->context == CONTEXT_CAVE)
        SetDynamicWarpFromDungeonCellWarp(enteredWarpId);
    else if (rootGameData->context == CONTEXT_DEFAULT) {}
    else fatal_assertf(FALSE);
}

void OnRunTasks_FnBegin(const u8 firstActiveTaskId)
{
    const struct TownDungeonGamePersistentData* gameData = GetTownDungeonGamePersistentData();

    if (gameData->context == CONTEXT_CAVE) CaveTick(255);
}

bool8 OnGetObjectObjectCollidesWith_OWECollisionBegin(struct ObjectEvent* objectEvent, s16 x, s16 y, bool32 addCoords, struct ObjectEvent* otherObject)
{
    if (!objectEvent->isPlayer && !otherObject->isPlayer) return TRUE; // if other things collide do nothing for now, but consume the event

    struct TownDungeonGamePersistentData* gameData = GetTownDungeonGamePersistentData();

    if (gameData->context == CONTEXT_CAVE)
    {
        struct TemporaryCaveState* temporaryCaveState = GetTemporaryCaveStatePtr(gameData);
        const struct CaveData* caveData = &GetCurrentTownDungeonData()->caveData;
        const struct DungeonCellData* currentCaveCellData = &caveData->cellsData[temporaryCaveState->currentCellIndex];

        const struct ObjectEvent* monsterObject = objectEvent->isPlayer ? otherObject : objectEvent;
        const u8 objectInstanceIndex = monsterObject - gObjectEvents;

        const struct ObjectIdentifier* objectIdentifier = &temporaryCaveState->objectIdByInstanceIndex[objectInstanceIndex];
        if (objectIdentifier->type == OBJ_ID_MONSTER)
        {
            if (!temporaryCaveState->battleQueued) 
            {
                temporaryCaveState->battleQueued = TRUE;

                // todo flag for if battle is already queued // todo reset where we resume root task from battle
                    // so that you can't call this twice
                StartCaveBattle(gameData, currentCaveCellData, objectIdentifier);
            }
            return TRUE;
        }
        if (objectIdentifier->type == OBJ_ID_PICKUP) return TRUE;
    }

    return FALSE;
}

void OnCB2_InitBattle_FnBegin(void)
{
    struct TownDungeonGamePersistentData* this = GetTownDungeonGamePersistentData();
    struct TemporaryCaveState* temporaryCaveState = GetTemporaryCaveStatePtr(this);
    const u8 defaultWaitDuration = 128;

    temporaryCaveState->battleQueued = FALSE;
    temporaryCaveState->aiWaitDuration = defaultWaitDuration;
}
