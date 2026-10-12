//
// Created by kubes on 10/12/26.
//
#include "mygame/util/map_layout.h"

#include "global.h"

#include "fieldmap.h"
#include "metatile_behavior.h"
#include "random.h"
#include "mygame/util/retry.h"

s32 GetTileIndex(const struct MapLayout* layout, const struct UCoords8 coordinate)
{
    return coordinate.x + coordinate.y * layout->width;
}

u16 GetTileMetatileId(const struct MapLayout* layout, const struct UCoords8 coordinate)
{
    const s32 blockIndex = GetTileIndex(layout, coordinate);

    return layout->map[blockIndex];
}

u32 GetTileMetatileBehavior(const struct MapLayout* layout, const struct UCoords8 coordinate)
{
    const u16 metatileId = GetTileMetatileId(layout, coordinate);

    return GetAttributeByMetatileIdAndMapLayout(metatileId, METATILE_ATTRIBUTE_BEHAVIOR, layout->isFrlg);
}

enum Collision GetTileCollisionType(const struct MapLayout* layout, const struct UCoords8 coordinate)
{
    const u16 tileMetatileId = GetTileMetatileId(layout, coordinate);

    return UNPACK_COLLISION(tileMetatileId);
}

bool8 TileIsPassable(const struct MapLayout* layout, const struct UCoords8 coordinate)
{
    return GetTileCollisionType(layout, coordinate) == COLLISION_NONE;
}

bool8 TileIsNorthwardWarp(const struct MapLayout* layout, const struct UCoords8 coordinate)
{
    const u32 metatileBehavior = GetTileMetatileBehavior(layout, coordinate);

    if (MetatileBehavior_IsNonAnimDoor(metatileBehavior)) return TRUE;
    if (MetatileBehavior_IsDoor(metatileBehavior)) return TRUE;
    if (MetatileBehavior_IsNorthArrowWarp(metatileBehavior)) return TRUE;

    return FALSE;
}

struct UCoords8 FindRandomTileByPredicate(const struct MapLayout* layout, const TileFunc predicate)
{
    const u8 maxAttempts = 29;
    struct UCoords8 returnValue;

    RETRY_CALL_FATAL(maxAttempts,
        returnValue.x = Random() % layout->width;
        returnValue.y = Random() % layout->height;
        ,
        predicate(layout, returnValue)
    )

    // ReSharper disable once CppLocalVariableMightNotBeInitialized
    return returnValue;
}

struct UCoords8 FindRandomPassableTile(const struct MapLayout* layout)
{
    return FindRandomTileByPredicate(layout, TileIsPassable);
}