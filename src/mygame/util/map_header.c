//
// Created by kubes on 10/7/26.
//
#include "mygame/util/map_header.h"

#include "global.h"

#include "overworld.h"
#include "mygame/util/map_layout.h"

#define FOR_EACH_WARP_EVENT(loopBodyForWarpEvent) \
    const struct MapHeader* const mapHeader = GetMapHeader(mapEnum); \
    const struct MapEvents* mapEvents = mapHeader->events; \
    \
    for (u8 i = 0; i < mapEvents->warpCount; i++) \
    { \
        const struct WarpEvent* warpEvent = &mapEvents->warps[i]; \
        \
        loopBodyForWarpEvent \
    }

static struct MapHeader const* GetMapHeader(const enum MapEnum mapEnum)
{
    return Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(mapEnum), MAP_NUM(mapEnum));
}

u8 GetAllWarps(const enum MapEnum mapEnum, struct WarpEvent* warpsArrOut, const u8 arrLength)
{
    const struct MapHeader* const mapHeader = GetMapHeader(mapEnum);
    const struct MapEvents* mapEvents = mapHeader->events;
    const u8 warpCount = mapEvents->warpCount;

    fatal_assertf(warpCount <= arrLength);

    for (u8 i = 0; i < warpCount; i++)
        warpsArrOut[i] = mapEvents->warps[i];

    return warpCount;
}

bool8 TileIsWarp(const enum MapEnum mapEnum, const struct UCoords8 coordinate)
{
    FOR_EACH_WARP_EVENT(
        if (warpEvent->x != coordinate.x) continue;
        if (warpEvent->y == coordinate.y) return TRUE;
    )

    return FALSE;
}


// only accounts for literal warp overlap and being under a northward warp
bool8 ObjectAtTileCouldBlockWarp(const enum MapEnum mapEnum, const struct UCoords8 coordinate)
{
    const struct MapLayout* mapLayout = GetMapHeader(mapEnum)->mapLayout;
    const struct UCoords8 potentialNorthwardDoorLocation = (struct UCoords8) {.x=coordinate.x, .y=coordinate.y + 1};

    FOR_EACH_WARP_EVENT(
        if (warpEvent->x != coordinate.x) continue;

        if (warpEvent->y == coordinate.y) return TRUE;

        if (warpEvent->y == potentialNorthwardDoorLocation.y
            && TileIsNorthwardWarp(mapLayout, potentialNorthwardDoorLocation))
        {
            return TRUE;
        }
    )

    return FALSE;
}

#undef FOR_EACH_WARP_EVENT