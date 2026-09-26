//
// Created by kubes on 9/26/26.
//
#include "mygame/persistent/town_dungeon_persistent_data.h"

// ReSharper disable once CppUseInternalLinkage
void OnTownMapLoaded()
{
    struct TownDungeonGamePersistentData* gameData = GetTownDungeonGamePersistentData();
    gameData->context = CONTEXT_TOWN;
}
