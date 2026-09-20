//
// Created by kubes on 9/19/26.
//
#ifndef GUARD_KUBES_TOWN_DUNGEON_PERSISTENT_DATA_H
#define GUARD_KUBES_TOWN_DUNGEON_PERSISTENT_DATA_H
#include "gba/types.h"

#define DUNGEON_TOWN_COUNT 16

struct TownDungeonPersistentData
{
    u64 dungeonTownData[DUNGEON_TOWN_COUNT]; // placeholder, uses under 10% of provided space
};
// if we run out of room in SaveBlock3 we can save some padding space by shucking this struct

#endif // GUARD_KUBES_TOWN_DUNGEON_PERSISTENT_DATA_H
