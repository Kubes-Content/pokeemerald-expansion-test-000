//
// Created by kubes on 9/26/26.
//
#include "mygame/patches/get_warp_id_under_player.h"

#include "global.h"
#include "field_control_avatar.h"

s8 GetWarpIdUnderPlayer()
{
    struct MapPosition playerPosition;
    GetPlayerPosition(&playerPosition);

    return GetWarpEventAtMapPosition(&gMapHeader, &playerPosition);
}
