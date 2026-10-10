//
// Created by kubes on 9/25/26.
//
#include "mygame/patches/dynamic_warp/dynamic_warp_pre_warp.h"

#include "mygame/patches/dynamic_warp/town_dungeon_warps.h"
#include "mygame/persistent/town_dungeon_persistent_data.h"

void PreDynamicWarpPatch(const u8 enteredWarpId)
{
    PreDynamicWarp(enteredWarpId);
}
