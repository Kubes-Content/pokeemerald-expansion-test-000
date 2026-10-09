//
// Created by kubes on 10/9/26.
//
#include "mygame/persistent/town_dungeon/cave/cell_pickup_config.h"

struct CellPickupConfig CellPickupConfig_Create(const u8 x, const u8 y, const u8 sharedDescriptionIndex)
{
    return (struct CellPickupConfig) {
        .isTaken = FALSE,
        .sharedDescriptionIndex = sharedDescriptionIndex,
        .x = x,
        .y = y,
    };
}
