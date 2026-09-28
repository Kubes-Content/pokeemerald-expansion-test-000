//
// Created by kubes on 9/21/26.
//
#include "mygame/persistent/town_dungeon_persistent_data.h"
#include "global.h"

#include "item.h"
#include "overworld.h"
#include "random.h"
#include "constants/event_objects.h"
#include "mygame/dungeon_generation/dungeon_generation_global.h"


struct TownDungeonGamePersistentData* GetTownDungeonGamePersistentData()
{
    return &gSaveBlock3Ptr->townDungeonData;
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

static struct CellVariant GenerateSingleCellStep(struct TownDungeonPersistentData* this, u8 newCellDataIndex,
                                                 const struct CellVariant* entryCell,
                                                 enum Connection directionFromEntryCell,
                                                 struct DummyPickupDescription pickupDescriptionArr[MAX_PICKUPS_PER_DUNGEON])
{
    const struct CellVariant newCell = GetConnectableCell(entryCell, RelativeCellConnection_Create(directionFromEntryCell));
    this->dungeonCellsData[newCellDataIndex] = DungeonCellData_Create(newCell.mapEnum, 3, pickupDescriptionArr); // TODO function is not responsible for literal integer

    return newCell;
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
static u8 GetAllRelativeCellConnections(struct CellVariant* cellVariant, struct RelativeCellConnection* out_connections, u8 connectionsArrayCapacity)
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

static struct RelativeCellConnection PickAnyOtherCellMapRelativeConnection(struct CellVariant* cellVariant, struct RelativeCellConnection unwantedConnection)
{
    const u8 connectionsCapacity = 4;
    struct RelativeCellConnection connections[connectionsCapacity];
    const u8 connectionsFound = GetAllRelativeCellConnections(cellVariant, connections, connectionsCapacity);

    const u8 chosenIndex = Random() % (connectionsFound - 1); // assuming that every connection is unique and none are like special clones that go to one location ... or something ...
    u8 chosenCount = 0;
    for (u8 i = 0; i < connectionsFound; i++)
    {
        if (RelativeCellConnection_Equal(&unwantedConnection, &connections[i]))
            continue;

        if (chosenCount == chosenIndex)
            return connections[i];

        chosenCount++;
    }
    fatalf("No result found for PickAnyOtherCellMapRelativeConnection");
}

// TODO this is useless until you can mark unused doorways
    // how does data know if an element of 'connections' is null?
    // does it have an length variable?
        // in that case are the indices not the connection's local warpId?
// todo fix: this assumes that entryCell is a room with a single pre-established connection to another cell
static void GenerateCavePathSegment(struct TownDungeonPersistentData* townData, u8* const generatedCellCount, const u8 rootPathMinimumLength, const struct CellVariant* entryCell, struct RelativeCellConnection connectionFromEntryToPrior, struct DummyPickupDescription pickupDescriptionArr[MAX_PICKUPS_PER_DUNGEON]) // NOLINT(*-non-const-parameter)
{
    // TODO generate a base path instead
        // randomly pick any direction other than the one that's already used
        // generate a new cell there
        // rinse and repeat until we hit half length, rounded up; would we need to multiply by 10 to get that?
    struct RelativeCellConnection connectionFromPreviousCellToPriorCell = connectionFromEntryToPrior;
    struct CellVariant previousCell = *entryCell;

    const u8 iteratorOffset = *generatedCellCount;
    for (u8 i = iteratorOffset; i < rootPathMinimumLength + iteratorOffset; i++)
    {
        const u8 previousCellIndex = *generatedCellCount - 1;
        const u8 newCellIndex      = *generatedCellCount;
        struct DummyDungeonCellData* previousCellData = &townData->dungeonCellsData[previousCellIndex];
        struct DummyDungeonCellData* newCellData      = &townData->dungeonCellsData[newCellIndex];

        // randomly pick an unused connection in previousCell
        const struct RelativeCellConnection connectionFromPreviousCellToNewCell = PickAnyOtherCellMapRelativeConnection(&previousCell, connectionFromPreviousCellToPriorCell);

        // todo extract this
        const struct RelativeCellConnection connectionFromNewCellToPreviousCell = RelativeCellConnection_Create(Reverse(connectionFromPreviousCellToNewCell.direction)); // reciprocal = toFrom

        const struct CellVariant newCell = GetConnectableCell(&previousCell, connectionFromPreviousCellToNewCell); // see: GenerateSingleCellStep

        // TODO simplify

        *newCellData = DungeonCellData_Create(newCell.mapEnum, 3, pickupDescriptionArr);

        const u8 previousCellFromWarpId = GetPlaceholderCellWarpIdForDirection(connectionFromPreviousCellToNewCell.direction);
        const u8 newCellToWarpId        = GetPlaceholderCellWarpIdForDirection(connectionFromNewCellToPreviousCell.direction);
        //
        previousCellData->connections[previousCellFromWarpId] = DungeonCellConnection_Create(newCellIndex, newCell.mapEnum, newCellToWarpId);
        newCellData->connections[newCellToWarpId]             = DungeonCellConnection_Create(previousCellIndex, previousCell.mapEnum, previousCellFromWarpId);

        connectionFromPreviousCellToPriorCell = connectionFromNewCellToPreviousCell;
        previousCell = newCell;
        (*generatedCellCount)++;
    }
}

// TODO this shouldn't be in this file
static void GenerateCaveData(struct TownDungeonPersistentData* this)
{
    const struct CellVariant entryCell = GetEntryCaveMap();
    u8 generatedCellCount = 0;

    struct DummyPickupDescription pickupDescriptionArr[MAX_PICKUPS_PER_DUNGEON];
    {
        pickupDescriptionArr[0] = PickupDescription_Create(0, ITEM_POTION, 1, 5 ,15, OBJ_EVENT_GFX_ITEM_BALL);
        pickupDescriptionArr[1] = PickupDescription_Create(0, ITEM_SUN_STONE, 1, 6 ,15, OBJ_EVENT_GFX_BALL_CUSHION);// amber crashes for some reason; just FRLG stuff?
        pickupDescriptionArr[2] = PickupDescription_Create(0, ITEM_SUPER_REPEL, 1, 7 ,15, OBJ_EVENT_GFX_KISS_CUSHION);
    }

    fatal_assertf(MAX_DUNGEON_CELL_COUNT >= this->dungeonCellMaxIndex + 1, "MAX_DUNGEON_CELL_COUNT too low for generation test");

    // TODO support entering cave from any direction, NOT TIED TO CARDINAL DIRECTION, abstracted to wrap cardinal direction until we implement a better solution in its place

    this->dungeonCellsData[generatedCellCount] = DungeonCellData_Create(entryCell.mapEnum, 3, pickupDescriptionArr); // todo reimplement, generate all cells
    //
    const s8 townCellIndex = -1; // todo this should probably be a constant of some form
    const u8 townDoorWarpId = 0; // one day this guy will be fetched dynamically. one day.
    const struct DungeonCellConnection connectionFromDungeonToTown = DungeonCellConnection_Create(townCellIndex, MAP_CAVE_TOWN_00, townDoorWarpId);
    //
    const u8 cell0ToTownConnectionIndex = 0; // TODO is this because 0 is the south warp id? idk, how would I know?
    this->dungeonCellsData[generatedCellCount].connections[cell0ToTownConnectionIndex] = connectionFromDungeonToTown; // duplicates enum and warpId used in GetEntryCell

    // wait. is connects' index literally the warp id?
        // shouldn't that be more explicit? like with a function? you monster.

    const u8 entryCellIndex = generatedCellCount;
    generatedCellCount++;

    {
        fatal_assertf(entryCell.hasSouthWarp);
        fatal_assertf(entryCell.hasNorthWarp);
        fatal_assertf(entryCell.hasWestWarp);
        fatal_assertf(entryCell.hasEastWarp);
    }

    // TODO extract path gen. loop with input ptr to generatedCellCount
        // so I could theoretically chain like this:
            // GeneratePathSegment (the below functionality)
            // GenerateForkInRoad
                // Fork0: GeneratePathSegment
                // Fork1: GenerateForkInRoad... etc.
        // and they all work from and iterate a relative index
    //struct DungeonCellConnection connectionFromPreviousCellToPriorCell = connectionFromDungeonToTown;

    struct RelativeCellConnection connectionFromPreviousCellToPriorCell = RelativeCellConnection_Create(CONNECTION_SOUTH);

    const u8 dungeonCellCount = this->dungeonCellMaxIndex + 1;
    const u8 rootPathMinimumLength = dungeonCellCount / 2 + dungeonCellCount % 2;

    GenerateCavePathSegment(this, &generatedCellCount, rootPathMinimumLength, &entryCell, connectionFromPreviousCellToPriorCell, pickupDescriptionArr);

    // TODO spawn the rest of the dungeon
    const u8 remainingCellCount = dungeonCellCount - rootPathMinimumLength;
}

static void InitializeTownDungeonConfig(struct TownDungeonPersistentData* this)
{
    this->dummyTownData;

    this->dungeonCellMaxIndex = MAX_DUNGEON_CELL_COUNT - 1;
    this->caveEntryCellMapWarpId = 0; // todo assuming entrance cell's entrance warp is id 0 // change

    GenerateCaveData(this);
}

void InitializeTownDungeonGameConfig(struct TownDungeonGamePersistentData* this)
{
    this->currentTown = 0;
    this->currentCellIndex = 0;
    struct TownDungeonPersistentData* initialTownData = GetCurrentTownDungeonData();
    InitializeTownDungeonConfig(initialTownData);
}

static struct DungeonCellConnection GetDungeonWarpDestination(const u8 enteredWarpId)
{
    // TODO we have to actually...
        // per warp save the destination map + id
            // and on both ends: from/to
    // TODO cell data should map destination via enteredWarpId

    const struct TownDungeonGamePersistentData* gameData = GetTownDungeonGamePersistentData();
    const struct TownDungeonPersistentData* townData = GetCurrentTownDungeonData();
    const struct DummyDungeonCellData* currentCellData = &townData->dungeonCellsData[gameData->currentCellIndex];
    return currentCellData->connections[enteredWarpId];
}

// TODO separate cell data to its own .c file

void SetDynamicWarpFromDungeonCellWarp(const u8 enteredWarpId)
{
    // TODO feed enteredWarpId into dungeonData to determine destination struct{MapEnum, WarpId}
    const struct DungeonCellConnection data = GetDungeonWarpDestination(enteredWarpId);
    const u16 destinationMapEnum = data.mapEnum;
    const s8 destinationWarpId   = (s8) data.warpId;
    GetTownDungeonGamePersistentData()->currentCellIndex = data.isTown ? 0 : data.cellIndex;

    SetDynamicWarp(0, MAP_GROUP(destinationMapEnum), MAP_NUM(destinationMapEnum), destinationWarpId);
}

void SetDynamicWarpFromDungeonTownWarp([[maybe_unused]] u8 enteredWarpId)
{
    struct TownDungeonGamePersistentData* gameData = GetTownDungeonGamePersistentData();
    const struct TownDungeonPersistentData* currentTownData = GetCurrentTownDungeonData();
    gameData->currentCellIndex = 0;
    const struct DummyDungeonCellData destinationCellData = currentTownData->dungeonCellsData[gameData->currentCellIndex];
    // todo ASSUMES that we always enter into first cell of a dungeon // extract behavior

    const u16 caveEntryCellMapEnum = destinationCellData.cellMapEnum; // todo use getter
    const s8 caveEntranceCellWarpId = currentTownData->caveEntryCellMapWarpId;
    SetDynamicWarp(0, MAP_GROUP(caveEntryCellMapEnum), MAP_NUM(caveEntryCellMapEnum), caveEntranceCellWarpId);
}
