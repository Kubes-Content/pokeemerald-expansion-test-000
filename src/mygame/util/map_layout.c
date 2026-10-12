//
// Created by kubes on 10/12/26.
//
#include "mygame/util/map_layout.h"

#include "global.h"
#include "random.h"
#include "mygame/util/retry.h"

s32 GetTileIndex(const struct MapLayout* layout, const struct UCoords8 coordinate)
{
    return coordinate.x + coordinate.y * layout->width;
}

enum Collision GetTileCollisionType(const struct MapLayout* layout, const struct UCoords8 coordinate)
{
    const s32 blockIndex = GetTileIndex(layout, coordinate);

    return UNPACK_COLLISION(layout->map[blockIndex]);
}

bool8 IsTilePassable(const struct MapLayout* layout, struct UCoords8 coordinate)
{
    return GetTileCollisionType(layout, coordinate) == COLLISION_NONE;
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
    return FindRandomTileByPredicate(layout, IsTilePassable);
}