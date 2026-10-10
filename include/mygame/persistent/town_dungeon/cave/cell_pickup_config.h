//
// Created by kubes on 10/9/26.
//
#ifndef GUARD_KUBES_CELL_PICKUP_DATA_H
#define GUARD_KUBES_CELL_PICKUP_DATA_H
#include "metaprogram.h"
#include "gba/types.h"
#include "mygame/patches/global/constants.h"
#include "mygame/town_dungeon_macros.h"

struct CellPickupConfig
{
    bool8 isTaken : BIT_SIZE(1);
    u8 sharedDescriptionIndex : BIT_SIZE(NUM_PICKUP_DESCRIPTIONS_PER_DUNGEON - 1);
    u8 x : BIT_SIZE(MAX_COORDINATE);
    u8 y : BIT_SIZE(MAX_COORDINATE);
};
struct CellPickupConfig CellPickupConfig_Create(u8 x, u8 y, u8 sharedDescriptionIndex);

#endif // GUARD_KUBES_CELL_PICKUP_DATA_H
