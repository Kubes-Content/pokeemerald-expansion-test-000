//
// Created by kubes on 9/21/26.
//
#include "global.h"
#include "event_object_movement.h"
#include "overworld.h"

#include "mygame/persistent/town_dungeon_persistent_data.h"

#include "event_data.h"
#include "item.h"
#include "constants/event_objects.h"


struct TownDungeonGamePersistentData* GetTownDungeonGamePersistentData()
{
    return &gSaveBlock3Ptr->townDungeonData;
}

struct TownDungeonPersistentData* GetCurrentTownDungeonData()
{
    struct TownDungeonGamePersistentData* this = GetTownDungeonGamePersistentData();
    return &this->dungeonTownData[this->currentTown];
}

static void InitializeTownDungeonConfig(struct TownDungeonPersistentData* this)
{
    this->dummyTownData;

    this->dungeonCellMaxIndex = MAX_DUNGEON_CELL_COUNT - 1;
    this->caveEntryCellMapWarpId = 0; // todo assuming entrance cell's entrance warp is id 0 // change

    struct DummyPickupDescription pickupDescriptionArr[MAX_PICKUPS_PER_DUNGEON];
    pickupDescriptionArr[0] = PickupDescription_Create(0, ITEM_POTION, 1, 5 ,15, OBJ_EVENT_GFX_ITEM_BALL);
    pickupDescriptionArr[1] = PickupDescription_Create(0, ITEM_SUN_STONE, 1, 6 ,15, OBJ_EVENT_GFX_BALL_CUSHION);// amber crashes for some reason; just FRLG stuff?
    pickupDescriptionArr[2] = PickupDescription_Create(0, ITEM_SUPER_REPEL, 1, 7 ,15, OBJ_EVENT_GFX_KISS_CUSHION);

    // temporary, warp to cave entrance room // todo randomize cave generation
    this->dungeonCellsData[0] = DungeonCellData_Create(MAP_CAVE_TOWN_DUNGEON_ROOM_TEST_01,
                                                       3,
                                                       pickupDescriptionArr); // duplicate over; refactor // todo reimplement, generate all cells
    this->dungeonCellsData[1] = DungeonCellData_Create(MAP_CAVE_TOWN_DUNGEON_ROOM_TEST_01,
                                                       3,
                                                       pickupDescriptionArr);
}

void InitializeTownDungeonGameConfig(struct TownDungeonGamePersistentData* this)
{
    this->currentTown = 0;
    this->currentCellIndex = 0;
    struct TownDungeonPersistentData* initialTownData = GetCurrentTownDungeonData();
    InitializeTownDungeonConfig(initialTownData);
}

void SetDynamicWarpFromDungeonCellWarp(const u8 enteredWarpId)
{
    // TODO I want both cells in dungeon to use the same map w/ unique state
        // just add a door on the left that warps to whichever you're not in

    //#; // TODO

    // data structure vs. initialization/gen.
    // if in cell 0 and warpId 0, return to town
    // TODO determine where we're warping to from cave cell
        // are we...
            // going back to town?
            // going to connected cell?

    if (enteredWarpId == 0) // TODO assuming that 0 is cave entrance
    {
        const u8 townEnum = MAP_CAVE_TOWN_00; // TODO fetch this instead
        const s8 caveEntranceInTownWarpId = 0;
        SetDynamicWarp(0, MAP_GROUP(townEnum), MAP_NUM(townEnum), caveEntranceInTownWarpId);
    }
    else
    {
        struct TownDungeonGamePersistentData* gameData = GetTownDungeonGamePersistentData();
        struct TownDungeonPersistentData* townData = GetCurrentTownDungeonData();

        // TODO ASSUMING EXACTLY TWO CELLS EXIST
        gameData->currentCellIndex = !gameData->currentCellIndex; // switch cells

        const struct DummyDungeonCellData* cellData = &townData->dungeonCellsData[gameData->currentCellIndex];
        const u8 mapEnum = cellData->cellMapEnum;
        const s8 sideRoomWarpId = 1; // TODO fetch
        SetDynamicWarp(0, MAP_GROUP(mapEnum), MAP_NUM(mapEnum), sideRoomWarpId);
    }
}

void SetDynamicWarpFromDungeonTownWarp(u8 enteredWarpId)
{
    struct TownDungeonGamePersistentData* gameData = GetTownDungeonGamePersistentData();
    const struct TownDungeonPersistentData* currentTownData = GetCurrentTownDungeonData();
    gameData->currentCellIndex = 0;
    const struct DummyDungeonCellData destinationCellData = currentTownData->dungeonCellsData[gameData->currentCellIndex];
    // todo ASSUMES that we always enter into first cell of a dungeon // extract behavior
    const s8 caveEntryCellMapEnum = destinationCellData.cellMapEnum; // todo use getter
    const s8 caveEntranceCellWarpId = currentTownData->caveEntryCellMapWarpId;
    SetDynamicWarp(0, MAP_GROUP(caveEntryCellMapEnum), MAP_NUM(caveEntryCellMapEnum), caveEntranceCellWarpId);
}
