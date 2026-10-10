//
// Created by kubes on 9/19/26.
//
#ifndef GUARD_KUBES_TOWN_DUNGEON_PERSISTENT_DATA_H
#define GUARD_KUBES_TOWN_DUNGEON_PERSISTENT_DATA_H
#include "gba/types.h"
#include "metaprogram.h"
#include "task.h"
#include "mygame/town_dungeon_macros.h"
#include "town_dungeon/cave_data.h"
#include "town_dungeon/town_data.h"

struct ObjectEvent;

struct TownDungeonPersistentData
{
    struct DummyTownData dummyTownData;
    s8 caveEntryCellMapWarpId : BIT_SIZE(31) + 1;
    struct CaveData caveData;
};

enum TownDungeonMapContext
{
    CONTEXT_DEFAULT,
    CONTEXT_TOWN,
    CONTEXT_CAVE,

    CONTEXTS_COUNT
};

union TemporaryStatePerContext
{
    u8 bytes[16 * 8];
    max_align_t align;
};

struct TownDungeonGamePersistentData
{
    union TemporaryStatePerContext temporaryStatePerContext;

    u8 rootTaskId : BIT_SIZE(NUM_TASKS);
    u8 currentTown : BIT_SIZE(DUNGEON_TOWN_COUNT - 1);
    enum TownDungeonMapContext context : BIT_SIZE(CONTEXTS_COUNT - 1);
    struct DummyPickupDescription sharedCavePickupDescriptions[NUM_PICKUP_DESCRIPTIONS_PER_DUNGEON];
    struct OverworldMonsterDescription sharedMonsterDescriptions[NUM_OVERWORLD_MONSTER_DESCRIPTIONS_PER_DUNGEON];
    struct TownDungeonPersistentData dungeonTownData[DUNGEON_TOWN_COUNT];
};

struct TownDungeonGamePersistentData* GetTownDungeonGamePersistentData(void);
struct TownDungeonPersistentData* GetCurrentTownDungeonData(void);
void InitializeTownDungeonGameConfig(struct TownDungeonGamePersistentData* this);
void ClearTemporaryStatePerContext(struct TownDungeonGamePersistentData* this);
bool8 RootTaskIsRunning(const struct TownDungeonGamePersistentData* this);
void KillRootTask(struct TownDungeonGamePersistentData* this);
u8 SetRootTask(struct TownDungeonGamePersistentData* this, TaskFunc func);

void PreDynamicWarp(u8 enteredWarpId);
void OnRunTasks_FnBegin(u8 firstActiveTaskId);
bool8 OnGetObjectObjectCollidesWith_OWECollisionBegin(struct ObjectEvent *objectEvent, s16 x, s16 y, bool32 addCoords, struct ObjectEvent* otherObject);
void OnCB2_InitBattle_FnBegin(void);

#endif // GUARD_KUBES_TOWN_DUNGEON_PERSISTENT_DATA_H
