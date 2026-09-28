//
// Created by kubes on 9/28/26.
//
#ifndef GUARD_KUBES_DUNGEON_GENERATION_GLOBAL_H
#define GUARD_KUBES_DUNGEON_GENERATION_GLOBAL_H
#include "mygame/global_with_beer_and_hookers.h"
#include "mygame/persistent/town_dungeon_persistent_data.h"

struct CellVariant
{
    u16 mapEnum : BIT_SIZE(MAP_COUNT - 1);
    bool8 hasNorthWarp : 1;
    bool8 hasSouthWarp : 1;
    bool8 hasEastWarp : 1;
    bool8 hasWestWarp : 1;
};
struct CellVariant CellVariant_CreateEmpty();
struct CellVariant CellVariant_Create(u16 mapEnum, bool8 hasNorthWarp, bool8 hasSouthWarp, bool8 hasEastWarp, bool8 hasWestWarp);

struct RelativeCellConnection
{
    enum Connection direction;
};
struct RelativeCellConnection RelativeCellConnection_Create(enum Connection direction);
bool8 RelativeCellConnection_Equal(const struct RelativeCellConnection* A, const struct RelativeCellConnection* B);

struct CellVariant GetConnectableCell(const struct CellVariant* A, struct RelativeCellConnection connectionType);

#endif // GUARD_KUBES_DUNGEON_GENERATION_GLOBAL_H
