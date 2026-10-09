//
// Created by kubes on 10/6/26.
//
#ifndef GUARD_KUBES_CELL_OW_MONSTER_DATA_H
#define GUARD_KUBES_CELL_OW_MONSTER_DATA_H
#include "metaprogram.h"
#include "gba/types.h"
#include "mygame/patches/global/constants.h"

// UCoords8 exists, but we can't include global.h since this is included in global_patches.h

struct CellOverworldMonsterConfig
{
    u8 sharedDescriptionIndex;
    u8 homeX : BIT_SIZE(MAX_COORDINATE);
    u8 homeY : BIT_SIZE(MAX_COORDINATE);
};
struct CellOverworldMonsterConfig CellOverworldMonsterConfig_Create(u8 homeX, u8 homeY, u8 sharedDescriptionIndex);

#endif // GUARD_KUBES_CELL_OW_MONSTER_DATA_H
