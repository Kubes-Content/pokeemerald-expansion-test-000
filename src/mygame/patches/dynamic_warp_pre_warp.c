//
// Created by kubes on 9/25/26.
//
#include "mygame/patches/dynamic_warp/dynamic_warp_pre_warp.h"

#include "global.h"

#include "overworld.h"
#include "constants/maps.h"
#include "mygame/persistent/town_dungeon_persistent_data.h"

void PreDynamicWarpPatch(const u8 warpId)
{
    const struct TownDungeonGamePersistentData* rootGameData = GetTownDungeonGamePersistentData();
    const struct TownDungeonPersistentData* currentTownData = GetCurrentTownDungeonData();

    // TODO map load could set a context enum (are we in the town or a cave cell)

    const u8 townEnum = MAP_CAVE_TOWN_00; // TODO fetch this instead
    if (rootGameData->context == CONTEXT_TOWN)
    {
        const s8 caveEntryCellMapEnum = currentTownData->dungeonCellsData[0].caveEntryCellMapEnum;
        SetDynamicWarp(0, MAP_GROUP(caveEntryCellMapEnum), MAP_NUM(caveEntryCellMapEnum), 0);
    }
    else if (rootGameData->context == CONTEXT_CAVE)
    {
        warpId;
        // todo use game data magic : route where it goes
        // data structure vs. initialization/gen.
        SetDynamicWarp(0, MAP_GROUP(townEnum), MAP_NUM(townEnum), 0);
        // TODO determine where we're warping to from cave
        // if in cell 0 and warpId 0, return to town; todo should use a WARP_ID macro in porymap
    }
    else if (rootGameData->context == CONTEXT_DEFAULT) {}
    else fatal_assertf(FALSE);
}