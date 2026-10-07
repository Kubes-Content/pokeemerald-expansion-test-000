//
// Created by kubes on 9/19/26.
//
#ifndef GUARD_KUBES_TOWN_DUNGEON_PERSISTENT_DATA_H
#define GUARD_KUBES_TOWN_DUNGEON_PERSISTENT_DATA_H
#include "gba/types.h"
#include "metaprogram.h"
#include "constants/event_objects.h"
#include "town_dungeon/cave_data.h"
#include "town_dungeon/town_data.h"

#define DUNGEON_TOWN_COUNT 2

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

    u8 currentTown : BIT_SIZE(DUNGEON_TOWN_COUNT - 1);
    enum TownDungeonMapContext context : BIT_SIZE(CONTEXTS_COUNT - 1);
    struct DummyPickupDescription sharedCavePickupDescriptions[NUM_PICKUP_DESCRIPTIONS_PER_CELL];
    struct TownDungeonPersistentData dungeonTownData[DUNGEON_TOWN_COUNT];
};

struct TownDungeonGamePersistentData* GetTownDungeonGamePersistentData(void);
struct TownDungeonPersistentData* GetCurrentTownDungeonData(void);
void InitializeTownDungeonGameConfig(struct TownDungeonGamePersistentData* this);

#endif // GUARD_KUBES_TOWN_DUNGEON_PERSISTENT_DATA_H
