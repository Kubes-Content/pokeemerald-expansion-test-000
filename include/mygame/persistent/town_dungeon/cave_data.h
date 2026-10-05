//
// Created by kubes on 10/4/26.
//
#ifndef GUARD_KUBES_TOWN_DUNGEON_CELL_DATA_H
#define GUARD_KUBES_TOWN_DUNGEON_CELL_DATA_H
#include "metaprogram.h"
#include "gba/types.h"
#include "constants/items.h"
#include "constants/event_objects.h"
#include "mygame/patches/global/constants.h"

// TODO move all function definitions to .c

#define MAX_DUNGEON_CELL_COUNT 8
#define MAX_PICKUPS_PER_DUNGEON 8
#define MAX_DUNGEON_CELL_CONNECTIONS_PER_CELL 4
#define NUM_PICKUP_DESCRIPTIONS_PER_CELL 8

struct DummyPickupDescription
{
    u16 itemEnum : BIT_SIZE(ITEMS_COUNT - 1); // should be able to give nothing so that I can use a pickup like a toggle or obstacle
    u16 objectEventGraphicsEnum : BIT_SIZE(NUM_OBJ_EVENT_GFX - 1);
};
struct DummyPickupDescription PickupDescription_Create(u16 itemEnum, u16 objectEventGraphicsEnum);

struct DungeonCellConnection
{
    bool8 isValid : 1;
    bool8 isTown  : 1;
    u8 cellIndex  : BIT_SIZE(MAX_DUNGEON_CELL_COUNT - 1);
    u8    warpId  : BIT_SIZE(127);
};
struct DungeonCellConnection DungeonCellConnection_Create(s8 cellIndex, u8 warpId);

struct DummyDungeonCellData
{
    u16 cellMapEnum : BIT_SIZE(MAP_COUNT - 1); // todo wrap in struct CellDescription
    u8 cellPickupDefinitionCount : BIT_SIZE(MAX_PICKUPS_PER_DUNGEON - 1);
    u8 takenPickups[MAX_PICKUPS_PER_DUNGEON];
    u8 relevantPickupIndices[MAX_PICKUPS_PER_DUNGEON];
    struct DungeonCellConnection connections[MAX_DUNGEON_CELL_CONNECTIONS_PER_CELL]; // todo initialize with new game
};
struct DummyDungeonCellData DungeonCellData_Create(u16 caveEntryCellMapEnum, s8 caveEntryCellPickupCount, const u8 dummyPickupDescriptionArr[NUM_PICKUP_DESCRIPTIONS_PER_CELL]);

struct CaveData
{
    u8 maxIndexForCell : BIT_SIZE(MAX_DUNGEON_CELL_COUNT - 1); // cell ct - 1
    u8 objectWithGateKeyStaticIndex : BIT_SIZE(OBJECT_EVENTS_COUNT - 1);
    struct DummyDungeonCellData cellsData[MAX_DUNGEON_CELL_COUNT];
};
struct CaveData CaveData_Create();

struct TownDungeonGamePersistentData;
struct DummyPickupDescription* GetPickupDescription(struct TownDungeonGamePersistentData* gameData, const struct DummyDungeonCellData* cellData, u8 index);

bool8* PickupIsTakenPtr(struct DummyDungeonCellData* cellData, u8 staticIndex);

#endif // GUARD_KUBES_TOWN_DUNGEON_CELL_DATA_H
