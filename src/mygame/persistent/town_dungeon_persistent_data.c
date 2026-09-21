//
// Created by kubes on 9/21/26.
//
#include "../../../include/global.h"
#include "../../../include/mygame/persistent/town_dungeon_persistent_data.h"

struct TownDungeonGamePersistentData* GetTownDungeonGamePersistentData()
{
    return &gSaveBlock3Ptr->townDungeonData;
}

struct TownDungeonPersistentData* GetCurrentTownDungeonData()
{
    struct TownDungeonGamePersistentData* this = GetTownDungeonGamePersistentData();
    return &this->dungeonTownData[this->currentTown];
}
