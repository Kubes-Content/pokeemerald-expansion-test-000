//
// Created by kubes on 9/19/26.
//
#ifndef GUARD_KUBES_TOWN_DUNGEON_PERSISTENT_DATA_H
#define GUARD_KUBES_TOWN_DUNGEON_PERSISTENT_DATA_H

#define DUNGEON_TOWN_COUNT 16

struct TownDungeonPersistentData
{
    u32 dummyTownData;
    u32 dummyDungeonData;
};

struct TownDungeonGamePersistentData
{
    u8 currentTown: BIT_SIZE(DUNGEON_TOWN_COUNT);
    struct TownDungeonPersistentData dungeonTownData[DUNGEON_TOWN_COUNT]; // placeholder, uses under 10% of provided space

};
// if we run out of room in SaveBlock3 we can save some padding space by shucking this struct

struct TownDungeonGamePersistentData* GetTownDungeonGamePersistentData();
struct TownDungeonPersistentData* GetCurrentTownDungeonData();

#endif // GUARD_KUBES_TOWN_DUNGEON_PERSISTENT_DATA_H
