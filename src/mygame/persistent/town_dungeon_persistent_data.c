//
// Created by kubes on 9/21/26.
//
#include "global.h"
#include "constants/map_event_ids.h"
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

// todo extract
// ReSharper disable once CppUseInternalLinkage
void SetDynamicWarpForCaveEntry() // player steps on space before door
{
    const struct TownDungeonPersistentData* this = GetCurrentTownDungeonData();
    const struct DummyDungeonCellData* dungeonCellData = &this->dungeonCellsData[0]; // refactor // todo get index from game data
    // for connections we will go through state somehow
    SetDynamicWarp(0, MAP_GROUP(dungeonCellData->caveEntryCellMapEnum), dungeonCellData->caveEntryCellMapEnum, dungeonCellData->caveEntryCellMapWarpId);
}

// ReSharper disable once CppUseInternalLinkage
void OnDynamicObjectInteractedWith()
{
    // ASSUMING: that this is an object in a dungeon // todo we need a more universal solution longterm
    const u8 currentlyInteractingObjectEvent = gSelectedObjectEvent; // instance id
    struct DummyDungeonCellData* cellData = &GetCurrentTownDungeonData()->dungeonCellsData[0]; // todo refactor, get current cell's index from game data
    const u8 pickupStaticIndex = cellData->staticPickupIndexByInstanceIndex[currentlyInteractingObjectEvent];

    struct DummyPickupDescription* pickupDescription = &cellData->dummyPickupDescription[pickupStaticIndex];

    gSpecialVar_Result = OBJECT_EVENTS_COUNT; // todo debug, do we still need this? check dynamic-interactable poryscript

    // pickup item
    pickupDescription->isTaken = TRUE;
    const u16 itemId = pickupDescription->itemEnum;
    AddBagItem(itemId, pickupDescription->quantity);

    // remove pickup object
    struct ObjectEvent* pickupObjectPtr = &gObjectEvents[currentlyInteractingObjectEvent];
    RemoveObjectEvent(pickupObjectPtr);
}

static void InitializeTownDungeonConfig(struct TownDungeonPersistentData* this)
{
    this->dummyTownData;

    this->dungeonCellMaxIndex = MAX_DUNGEON_CELL_COUNT - 1;

    struct DummyPickupDescription pickupDescriptionArr[MAX_PICKUPS_PER_DUNGEON];
    pickupDescriptionArr[0] = PickupDescription_Create(0, ITEM_POTION, 1, 5 ,15, OBJ_EVENT_GFX_ITEM_BALL);
    pickupDescriptionArr[1] = PickupDescription_Create(0, ITEM_SUN_STONE, 1, 6 ,15, OBJ_EVENT_GFX_BALL_CUSHION);// amber crashes for some reason; just FRLG stuff?
    pickupDescriptionArr[2] = PickupDescription_Create(0, ITEM_SUPER_REPEL, 1, 7 ,15, OBJ_EVENT_GFX_KISS_CUSHION);

    // temporary, warp to cave entrance room // todo make entry room random
    this->dungeonCellsData[0] = DungeonCellData_Create(MAP_CAVE_TOWN_DUNGEON_ROOM_TEST_01,
                                                       0,
                                                       3,
                                                       pickupDescriptionArr); // duplicate over; refactor // todo reimplement, generate all cells
    this->dungeonCellsData[1] = DungeonCellData_Create(MAP_CAVE_TOWN_00,
                                                       0,
                                                       3,
                                                       pickupDescriptionArr);
}

void InitializeTownDungeonGameConfig(struct TownDungeonGamePersistentData* this)
{
    this->currentTown = 0;
    struct TownDungeonPersistentData* initialTownData = GetCurrentTownDungeonData();
    InitializeTownDungeonConfig(initialTownData);
}