//
// Created by kubes on 9/25/26.
//
#include "mygame/patches/dynamic_warp/dynamic_warp_pre_warp.h"

#include "global.h"

#include "overworld.h"
#include "constants/maps.h"
#include "mygame/patches/dynamic_warp/town_dungeon_warps.h"

void PreDynamicWarpPatch(const u8 enteredWarpId)
{
    const struct TownDungeonGamePersistentData* rootGameData = GetTownDungeonGamePersistentData();

    // todo extract this behavior to town_dungeon_persistent_data.c
    if (rootGameData->context == CONTEXT_TOWN)
        SetDynamicWarpFromDungeonTownWarp(enteredWarpId);
    else if (rootGameData->context == CONTEXT_CAVE)
        SetDynamicWarpFromDungeonCellWarp(enteredWarpId);
    else if (rootGameData->context == CONTEXT_DEFAULT) {}
    else fatal_assertf(FALSE);
}