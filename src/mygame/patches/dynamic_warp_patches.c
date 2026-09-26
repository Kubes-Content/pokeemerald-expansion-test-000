//
// Created by kubes on 9/25/26.
//
#include "mygame/patches/dynamic_warp_patches.h"

#include "mygame/patches/get_warp_id_under_player.h"
#include "mygame/patches/dynamic_warp/dynamic_warp_pre_warp.h"

void SetWarpDestinationToDynamicWarp_FnBegin()
{
   PreDynamicWarpPatch(GetWarpIdUnderPlayer());
}
