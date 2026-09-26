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

#define DUNGEON_TOWN_COUNT 2
#define MAX_DUNGEON_CELL_COUNT 2
#define MAX_PICKUPS_PER_DUNGEON 8

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
struct DummyDungeonCellData
{
    s8 caveEntryCellMapEnum;
    s8 caveEntryCellMapWarpId : BIT_SIZE(31) + 1;
    u8 caveEntryCellPickupDefinitionCount : BIT_SIZE(MAX_PICKUPS_PER_DUNGEON - 1);
    struct DummyPickupDescription dummyPickupDescription[MAX_PICKUPS_PER_DUNGEON];
    u8 staticPickupIndexByInstanceIndex[MAX_PICKUPS_PER_DUNGEON]; // todo you could pack these to fit in half of the size
};

// TODO move all function definitions to .c

inline struct DummyTownData TownData_Create()
{
    return (struct DummyTownData) {};
}
inline struct DummyDungeonCellData DungeonCellData_Create(s8 caveEntryCellMapEnum, s8 caveEntryCellMapWarpId, s8 caveEntryCellPickupCount, const struct DummyPickupDescription* dummyPickupDescriptionArr)
{
    struct DummyDungeonCellData result = {
        .caveEntryCellMapEnum = caveEntryCellMapEnum,
        .caveEntryCellMapWarpId = caveEntryCellMapWarpId,
        .caveEntryCellPickupDefinitionCount = caveEntryCellPickupCount,
    };
    memcpy(&result.dummyPickupDescription, dummyPickupDescriptionArr, sizeof(result.dummyPickupDescription));
    return result;
}
inline struct DummyPickupDescription PickupDescription_Create(u8 isTaken, u16 itemEnum, u8 quantity, s16 x, s16 y, u16 objectEventGraphicsEnum)
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

struct TownDungeonPersistentData
{
    struct DummyTownData dummyTownData;
    u8 dungeonCellMaxIndex : BIT_SIZE(MAX_DUNGEON_CELL_COUNT - 1); // cell ct - 1
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
    enum TownDungeonMapContext context : BIT_SIZE(CONTEXTS_COUNT - 1);
    struct TownDungeonPersistentData dungeonTownData[DUNGEON_TOWN_COUNT];

};
// if we run out of room in SaveBlock3 we can save some padding space by shucking some structs

struct TownDungeonGamePersistentData* GetTownDungeonGamePersistentData(void);
struct TownDungeonPersistentData* GetCurrentTownDungeonData(void);
void InitializeTownDungeonGameConfig(struct TownDungeonGamePersistentData* this);

#endif // GUARD_KUBES_TOWN_DUNGEON_PERSISTENT_DATA_H
