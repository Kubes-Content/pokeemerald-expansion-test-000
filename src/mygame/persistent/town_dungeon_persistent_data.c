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
static u8 GetAllRelativeCellConnections(const struct CellVariant* cellVariant, struct RelativeCellConnection* out_connections, u8 connectionsArrayCapacity)
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

// TODO this is useless until you can mark unused doorways
    // how does data know if an element of 'connections' is null?
    // does it have an length variable?
        // in that case are the indices not the connection's local warpId?
// todo fix: this assumes that entryCell is a room with a single pre-established connection to another cell
// unwantedConnections is relative to entry/owning cell
static void GenerateCavePathSegment(struct TownDungeonPersistentData* townData, u8* const generatedCellCount, struct CellVariant caveCellVariants[MAX_DUNGEON_CELL_COUNT], const u8 segmentCellCount, const struct CellVariant* entryCell, const u8 entryCellIndex, struct RelativeCellConnection* unwantedConnections, u8 unwantedConnectionsCount, struct DummyPickupDescription pickupDescriptionArr[MAX_PICKUPS_PER_DUNGEON]) // NOLINT(*-non-const-parameter)
{
    struct CaveData* caveData = &townData->caveData;

    const struct CellVariant* previousCell = entryCell;
    u8 previousCellIndex = entryCellIndex;

    const u8 iteratorOffset = *generatedCellCount;
    for (u8 i = iteratorOffset; i < segmentCellCount + iteratorOffset; i++)
    {
        fatal_assertf(*generatedCellCount < MAX_DUNGEON_CELL_COUNT, "Generating %d with a max of %d", *generatedCellCount + 1, (u8) MAX_DUNGEON_CELL_COUNT);
        const u8 newCellIndex = *generatedCellCount;
        struct DummyDungeonCellData* previousCellData = &caveData->dungeonCellsData[previousCellIndex];
        struct DummyDungeonCellData* newCellData = &caveData->dungeonCellsData[newCellIndex];

        // randomly pick an unused connection in previousCell

        const struct RelativeCellConnection connectionFromPreviousCellToNewCell = PickAnyOtherCellMapRelativeConnection(previousCell, unwantedConnections, unwantedConnectionsCount);

        // todo extract this
        const struct RelativeCellConnection connectionFromNewCellToPreviousCell = RelativeCellConnection_Create(Reverse(connectionFromPreviousCellToNewCell.direction)); // reciprocal = toFrom

        caveCellVariants[newCellIndex] = GetConnectableCell(previousCell, connectionFromPreviousCellToNewCell);
        const struct CellVariant* newCell = &caveCellVariants[newCellIndex]; // see: GenerateSingleCellStep

        // TODO simplify

        *newCellData = DungeonCellData_Create(newCell->mapEnum, 3, pickupDescriptionArr);

        const u8 previousCellFromWarpId = GetPlaceholderCellWarpIdForDirection(connectionFromPreviousCellToNewCell.direction);
        const u8 newCellToWarpId        = GetPlaceholderCellWarpIdForDirection(connectionFromNewCellToPreviousCell.direction);
        //
        previousCellData->connections[previousCellFromWarpId] = DungeonCellConnection_Create(newCellIndex, newCellToWarpId);
        newCellData->connections[newCellToWarpId]             = DungeonCellConnection_Create(previousCellIndex, previousCellFromWarpId);

        previousCell = newCell;
        previousCellIndex = newCellIndex;

        unwantedConnectionsCount = 1;
        unwantedConnections[0] = connectionFromNewCellToPreviousCell;
        (*generatedCellCount)++;
    }
}

// TODO this shouldn't be in this file
static void GenerateCaveData(struct TownDungeonPersistentData* this)
{
    struct CaveData* caveData = &this->caveData;

    struct CellVariant caveCellVariants[MAX_DUNGEON_CELL_COUNT];
    caveCellVariants[0] = GetEntryCaveMap();

    const struct CellVariant* entryCell = &caveCellVariants[0];
    u8 generatedCellCount = 0;

    struct DummyPickupDescription pickupDescriptionArr[MAX_PICKUPS_PER_DUNGEON];
    {
        pickupDescriptionArr[0] = PickupDescription_Create(0, ITEM_POTION, 1, 5 ,15, OBJ_EVENT_GFX_ITEM_BALL);
        pickupDescriptionArr[1] = PickupDescription_Create(0, ITEM_SUN_STONE, 1, 6 ,15, OBJ_EVENT_GFX_BALL_CUSHION);// amber crashes for some reason; just FRLG stuff?
        pickupDescriptionArr[2] = PickupDescription_Create(0, ITEM_SUPER_REPEL, 1, 7 ,15, OBJ_EVENT_GFX_KISS_CUSHION);
    }

    fatal_assertf(MAX_DUNGEON_CELL_COUNT >= caveData->dungeonCellMaxIndex + 1, "MAX_DUNGEON_CELL_COUNT too low for generation test");

    // TODO support entering cave from any direction, NOT TIED TO CARDINAL DIRECTION, abstracted to wrap cardinal direction until we implement a better solution in its place

    caveData->dungeonCellsData[generatedCellCount] = DungeonCellData_Create(entryCell->mapEnum, 3, pickupDescriptionArr); // todo reimplement, generate all cells
    //
    const s8 townCellIndex = -1; // todo this should probably be a constant of some form
    const u8 townDoorWarpId = 0; // one day this guy will be fetched dynamically. one day.
    const struct DungeonCellConnection connectionFromDungeonToTown = DungeonCellConnection_Create(townCellIndex, townDoorWarpId);
    //
    const u8 cell0ToTownConnectionIndex = 0; // TODO is this because 0 is the south warp id? idk, how would I know?
    caveData->dungeonCellsData[generatedCellCount].connections[cell0ToTownConnectionIndex] = connectionFromDungeonToTown; // duplicates enum and warpId used in GetEntryCell

    // wait. is connects' index literally the warp id?
    // shouldn't that be more explicit? like with a function? you monster.

    {
        fatal_assertf(entryCell->hasSouthWarp);
        fatal_assertf(entryCell->hasNorthWarp);
        fatal_assertf(entryCell->hasWestWarp);
        fatal_assertf(entryCell->hasEastWarp);
    }

    const u8 dungeonCellCount = caveData->dungeonCellMaxIndex + 1;
    const u8 rootPathMinimumLength = dungeonCellCount / 2 + dungeonCellCount % 2;

    {

        const u8 entryCellIndex = generatedCellCount;
        generatedCellCount++;

        // TODO extract path gen. loop with input ptr to generatedCellCount
            // so I could theoretically chain like this:
                // GeneratePathSegment (the below functionality)
                // GenerateForkInRoad
                    // Fork0: GeneratePathSegment
                    // Fork1: GenerateForkInRoad... etc.
            // and they all work from and iterate a relative index
        //struct DungeonCellConnection connectionFromPreviousCellToPriorCell = connectionFromDungeonToTown;

        const struct RelativeCellConnection connectionFromPreviousCellToPriorCell = RelativeCellConnection_Create(CONNECTION_SOUTH);

        const u8 unwantedConnectionsCount = 1;
        struct RelativeCellConnection unwantedConnections[unwantedConnectionsCount];
            unwantedConnections[0] = connectionFromPreviousCellToPriorCell;
        const u8 cellsToGenerateForRootPath = rootPathMinimumLength - 1; // first cell already generated
        GenerateCavePathSegment(this, &generatedCellCount, caveCellVariants, cellsToGenerateForRootPath, entryCell, entryCellIndex, unwantedConnections, unwantedConnectionsCount, pickupDescriptionArr);
    }

    // GEN SUB-PATHS

    // TODO spawn the rest of the dungeon
    const u8 remainingCellCount = dungeonCellCount - rootPathMinimumLength;

    for (u8 sideRoomIndex = 0; sideRoomIndex < remainingCellCount; sideRoomIndex++)
    {
        const u8 maximumAttempts = 99;
        u8 tryCount = 0;

redo:
        fatal_assertf(tryCount < maximumAttempts);
        tryCount++;

        const u8 ownerCellIndex = Random() % rootPathMinimumLength;
        struct DummyDungeonCellData* ownerCellData = &caveData->dungeonCellsData[ownerCellIndex];
        struct CellVariant* ownerCellVariant = &caveCellVariants[ownerCellIndex];
        // TODO get owner's variant
            // do I have to keep an array of each cellvariant to rereference via cell index?
        // TODO assert owner has an unused door
        // TODO determine an unused door in owner cell (or find a new owner)

        // TODO can I just get the connections array length? count how many are unused and pick a random one?
        //struct DungeonCellConnection* chosenConnectionFromOwner;
        //struct RelativeCellDirection connectionFromPreviousCellToNewCell;

        //if (!GetUnusedGenTimeDirection(ownerCellData, &a)) goto redo;

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

            /*unusedConnectionsCount = 0;
            const u8 chosenConnectionIndex = Random() % unusedConnectionsCount;
            for (u8 i = 0; i < MAX_DUNGEON_CELL_CONNECTIONS_PER_CELL; i++)
            {
                struct DungeonCellConnection* dungeonCellConnection = &ownerCellData->connections[i];
                if (dungeonCellConnection->isValid) continue;

                if (unusedConnectionsCount == chosenConnectionIndex)
                {
                    chosenConnectionFromOwner = dungeonCellConnection;
                    break;
                }

                unusedConnectionsCount++;
            }*/
        }
        // TODO now how the hell do I get from a DungeonCellConnection to a RelativeCellConnection?
            // without just assuming cardinal directions or warpId indices or anything
            // RelativeCellConnection is only used while generating, right?
                // is a cardinal direction; wrapped to make cardinal directions easier to move away from
                // used in both FromThisCell->ToOtherCell and FromOtherCell->ToThisCell contexts
            // DungeonCellConnection is saved data
                // simply says where the related warp goes within cave data

        // TODO dependsOnChosenRelativeConnection;
        /*const struct RelativeCellConnection connectionFromPreviousCellToNewCell =
            PickAnyOtherCellMapRelativeConnection(ownerCellVariant, unwantedConnectionsArr, unwantedConnectionsCount);
        GetConnectableCell(ownerCellVariant, connectionFromPreviousCellToNewCell);*/
        const u8 subPathLength = 1;
        GenerateCavePathSegment(this, &generatedCellCount, caveCellVariants, subPathLength, ownerCellVariant, ownerCellIndex, unwantedConnectionsArr, unwantedConnectionsCount, pickupDescriptionArr);
    }
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
    this->currentCellIndex = 0;
    struct TownDungeonPersistentData* initialTownData = GetCurrentTownDungeonData();
    InitializeTownDungeonConfig(initialTownData);
}