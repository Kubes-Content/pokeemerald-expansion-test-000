//
// Created by kubes on 9/19/26.
//
#ifndef GUARD_KUBES_TOWN_DUNGEON_PERSISTENT_DATA_H
#define GUARD_KUBES_TOWN_DUNGEON_PERSISTENT_DATA_H
#include "gba/types.h"
#include "metaprogram.h"
#include "constants/event_objects.h"
#include "constants/items.h"
#include "string.h"
#include "mygame/patches/global_patches.h"

#define DUNGEON_TOWN_COUNT 2
#define MAX_DUNGEON_CELL_COUNT 4
#define MAX_PICKUPS_PER_DUNGEON 8
#define MAX_DUNGEON_CELL_CONNECTIONS_PER_CELL 4

struct DummyTownData
{
    // todo
};
struct DummyPickupDescription
{
    u8 isTaken : 1;
    u16 itemEnum : BIT_SIZE(ITEMS_COUNT - 1); // should be able to give nothing so that I can use a pickup like a toggle or obstacle
    u8 quantity;
    s16 x : BIT_SIZE(32 - 1) + 1; // todo better define our max dungeon cell extents
    s16 y : BIT_SIZE(32 - 1) + 1;
    u16 objectEventGraphicsEnum : BIT_SIZE(NUM_OBJ_EVENT_GFX - 1);
};
struct DungeonCellConnection
{
    bool8 isValid : 1;
    bool8 isTown  : 1;
    u8 cellIndex  : BIT_SIZE(MAX_DUNGEON_CELL_COUNT - 1);
    u16   mapEnum : BIT_SIZE(MAP_COUNT - 1); // todo shouldn't this be derived from the cellIndex/data? // cuts this from 6 to 2 bytes
    u8    warpId  : BIT_SIZE(127);
};
static inline struct DungeonCellConnection DungeonCellConnection_Create(const s8 cellIndex, const u16 mapEnum, const u8 warpId)
{
    return (struct DungeonCellConnection) {
        .isValid = TRUE,
        .isTown = cellIndex < 0,
        .cellIndex = cellIndex,
        .mapEnum = mapEnum,
        .warpId = warpId,
    };
}

struct DummyDungeonCellData
{
    u16 cellMapEnum : BIT_SIZE(MAP_COUNT - 1);
    u8 cellPickupDefinitionCount : BIT_SIZE(MAX_PICKUPS_PER_DUNGEON - 1);
    struct DummyPickupDescription dummyPickupDescription[MAX_PICKUPS_PER_DUNGEON];
    struct DungeonCellConnection connections[MAX_DUNGEON_CELL_CONNECTIONS_PER_CELL]; // todo initialize with new game
};

// TODO move all function definitions to .c

// ReSharper disable CppRedundantInlineSpecifier
static inline struct DummyTownData TownData_Create()
{
    return (struct DummyTownData) {};
}

static inline struct DummyDungeonCellData DungeonCellData_Create(const u16 caveEntryCellMapEnum, const s8 caveEntryCellPickupCount, const struct DummyPickupDescription dummyPickupDescriptionArr[MAX_PICKUPS_PER_DUNGEON])
{
    struct DummyDungeonCellData result = {
        .cellMapEnum = caveEntryCellMapEnum,
        .cellPickupDefinitionCount = caveEntryCellPickupCount,
    };
    memcpy(&result.dummyPickupDescription, dummyPickupDescriptionArr, sizeof(result.dummyPickupDescription));
    memset(&result.connections, 0, sizeof(result.connections));

    return result;
}
static inline struct DummyPickupDescription PickupDescription_Create(const u8 isTaken, const u16 itemEnum, const u8 quantity, const s16 x, const s16 y, const u16 objectEventGraphicsEnum)
{
    return (struct DummyPickupDescription) {
        .isTaken = isTaken,
        .itemEnum = itemEnum,
        .quantity = quantity,
        .x = x,
        .y = y,
        .objectEventGraphicsEnum = objectEventGraphicsEnum,
    };
}
// ReSharper restore CppRedundantInlineSpecifier

struct TownDungeonPersistentData
{
    struct DummyTownData dummyTownData;
    u8 dungeonCellMaxIndex : BIT_SIZE(MAX_DUNGEON_CELL_COUNT - 1); // cell ct - 1
    s8 caveEntryCellMapWarpId : BIT_SIZE(31) + 1;
    u8 objectHoldingGateKeyStaticIndex : BIT_SIZE(OBJECT_EVENTS_COUNT - 1);
    struct DummyDungeonCellData dungeonCellsData[MAX_DUNGEON_CELL_COUNT];
};

enum TownDungeonMapContext
{
    CONTEXT_DEFAULT,
    CONTEXT_TOWN,
    CONTEXT_CAVE,

    CONTEXTS_COUNT
};

struct TownDungeonGamePersistentData
{
    u8 currentTown : BIT_SIZE(DUNGEON_TOWN_COUNT - 1);
    u8 currentCellIndex : BIT_SIZE(MAX_DUNGEON_CELL_COUNT - 1);
    u8 objectStaticPickupIndexByInstanceIndex[OBJECT_EVENTS_COUNT]; // todo you could pack these to fit in half of the size
    enum TownDungeonMapContext context : BIT_SIZE(CONTEXTS_COUNT - 1);
    struct TownDungeonPersistentData dungeonTownData[DUNGEON_TOWN_COUNT];

};
// if we run out of room in SaveBlock3
    // besides reducing DUNGEON_TOWN_COUNT...
    // we can save some space by...
        // shucking structs to reduce padding
        // use VarSet/VarGet for variables that won't need multiple instances
            // just be careful of potential pre-existing usages of the address

struct TownDungeonGamePersistentData* GetTownDungeonGamePersistentData(void);
struct TownDungeonPersistentData* GetCurrentTownDungeonData(void);
void InitializeTownDungeonGameConfig(struct TownDungeonGamePersistentData* this);

void SetDynamicWarpFromDungeonCellWarp(u8 enteredWarpId);
void SetDynamicWarpFromDungeonTownWarp(u8 enteredWarpId);

#endif // GUARD_KUBES_TOWN_DUNGEON_PERSISTENT_DATA_H
