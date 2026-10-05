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

struct DummyPickupDescription
{
    u8 isTaken : 1;
    u16 itemEnum : BIT_SIZE(ITEMS_COUNT - 1); // should be able to give nothing so that I can use a pickup like a toggle or obstacle
    u8 quantity;
    s16 x : BIT_SIZE(32 - 1) + 1; // todo better define our max dungeon cell extents
    s16 y : BIT_SIZE(32 - 1) + 1;
    u16 objectEventGraphicsEnum : BIT_SIZE(NUM_OBJ_EVENT_GFX - 1);
};
struct DummyPickupDescription PickupDescription_Create(u8 isTaken, u16 itemEnum, u8 quantity, s16 x, s16 y, u16 objectEventGraphicsEnum);

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
    u16 cellMapEnum : BIT_SIZE(MAP_COUNT - 1);
    u8 cellPickupDefinitionCount : BIT_SIZE(MAX_PICKUPS_PER_DUNGEON - 1);
    struct DummyPickupDescription dummyPickupDescription[MAX_PICKUPS_PER_DUNGEON];
    struct DungeonCellConnection connections[MAX_DUNGEON_CELL_CONNECTIONS_PER_CELL]; // todo initialize with new game
};
struct DummyDungeonCellData DungeonCellData_Create(u16 caveEntryCellMapEnum, s8 caveEntryCellPickupCount, const struct DummyPickupDescription dummyPickupDescriptionArr[MAX_PICKUPS_PER_DUNGEON]);

struct CaveData
{
    u8 dungeonCellMaxIndex : BIT_SIZE(MAX_DUNGEON_CELL_COUNT - 1); // cell ct - 1
    u8 objectHoldingGateKeyStaticIndex : BIT_SIZE(OBJECT_EVENTS_COUNT - 1);
    struct DummyDungeonCellData dungeonCellsData[MAX_DUNGEON_CELL_COUNT];
};
struct CaveData CaveData_Create();

#endif // GUARD_KUBES_TOWN_DUNGEON_CELL_DATA_H
