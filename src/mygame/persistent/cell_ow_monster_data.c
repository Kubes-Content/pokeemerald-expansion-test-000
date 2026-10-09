//
// Created by kubes on 10/8/26.
//
#include "mygame/persistent/town_dungeon/cave/cell_ow_monster_data.h"

struct CellOverworldMonsterConfig CellOverworldMonsterConfig_Create(const u8 homeX, const u8 homeY, const u8 sharedDescriptionIndex)
{
    return (struct CellOverworldMonsterConfig) {
        .sharedDescriptionIndex = sharedDescriptionIndex,
        .homeX = homeX,
        .homeY = homeY,
    };
}
