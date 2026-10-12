//
// Created by kubes on 10/12/26.
//
#ifndef GUARD_KUBES_MAP_LAYOUT_H
#define GUARD_KUBES_MAP_LAYOUT_H
#include "gba/types.h"
struct MapLayout;
struct UCoords8;

typedef bool8 (*TileFunc)(const struct MapLayout* layout, struct UCoords8 coordinate);

s32             GetTileIndex           (const struct MapLayout* layout, struct UCoords8 coordinate);
u16             GetTileMetatileId      (const struct MapLayout* layout, struct UCoords8 coordinate);
u32             GetTileMetatileBehavior(const struct MapLayout* layout, struct UCoords8 coordinate);
enum Collision  GetTileCollisionType   (const struct MapLayout* layout, struct UCoords8 coordinate);

bool8           TileIsPassable         (const struct MapLayout* layout, struct UCoords8 coordinate);
bool8           TileIsNorthwardWarp    (const struct MapLayout* layout, struct UCoords8 coordinate);

struct UCoords8 FindRandomTileByPredicate (const struct MapLayout* layout, TileFunc predicate);
struct UCoords8 FindRandomPassableTile    (const struct MapLayout* layout);

#endif // GUARD_KUBES_MAP_LAYOUT_H
