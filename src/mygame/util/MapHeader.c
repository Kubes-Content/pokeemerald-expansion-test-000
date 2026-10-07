//
// Created by kubes on 10/7/26.
//
#include "mygame/util/MapHeader.h"

#include "global.h"
#include "overworld.h"

u8 GetAllWarps(const enum MapEnum mapEnum, struct WarpEvent* warpsArr, u8 arrLength)
{
    const struct MapHeader* const mapHeader = Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(mapEnum), MAP_NUM(mapEnum));
    const struct MapEvents* mapEvents = mapHeader->events;
    const u8 warpCount = mapEvents->warpCount;

    fatal_assertf(warpCount <= arrLength);

    for (u8 i = 0; i < warpCount; i++) // todo ctor CellVariantRestrictions
        warpsArr[i] = mapEvents->warps[i];

    return warpCount;
}
