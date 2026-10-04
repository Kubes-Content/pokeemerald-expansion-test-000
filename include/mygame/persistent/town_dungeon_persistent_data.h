//
// Created by kubes on 9/19/26.
//
#ifndef GUARD_KUBES_TOWN_DUNGEON_PERSISTENT_DATA_H
#define GUARD_KUBES_TOWN_DUNGEON_PERSISTENT_DATA_H
#include "gba/types.h"
#include "metaprogram.h"
#include "constants/event_objects.h"
#include "town_dungeon_cell_data.h"

#define DUNGEON_TOWN_COUNT 2

struct DummyTownData
{
    // todo
};

// TODO move all function definitions to .c

// ReSharper disable CppRedundantInlineSpecifier
static inline struct DummyTownData TownData_Create()
{
    return (struct DummyTownData) {};
}

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
