//
// Created by kubes on 10/16/26.
//
#ifndef GUARD_KUBES_GROUND_OBJECT_STATE_H
#define GUARD_KUBES_GROUND_OBJECT_STATE_H
#include "metaprogram.h"
#include "gba/types.h"

#define GROUND_OBJECT_STATE_SIZE    2
#define GROUND_OBJECT_STATE_PADDING 1

enum PACKED GroundObjectType
{
    GROUND_OBJECT_TYPE_UNINITIALIZED,

    GROUND_OBJECT_TYPE_FRUIT,

    GROUND_OBJECT_TYPE_COUNT // if we really needed to save one enum value, we could put above enum list into a macro (with prefix arg) .. then make two enums, one with the _COUNT, one without. We only use the one without _COUNT in code, but use the _COUNT from the other
};

struct PACKED GroundObjectState
{
    enum GroundObjectType groundObjectType : BIT_SIZE(GROUND_OBJECT_TYPE_COUNT);
    u16 filler : GROUND_OBJECT_STATE_SIZE * 8 - BIT_SIZE(GROUND_OBJECT_TYPE_COUNT) - GROUND_OBJECT_STATE_PADDING;
    u16 variant : GROUND_OBJECT_STATE_PADDING; // TODO default to zero // document what this is :) because I forget :) initially for bIsBuried?
};
_Static_assert(sizeof(struct GroundObjectState) == GROUND_OBJECT_STATE_SIZE, "incorrect struct size, %d != %d");
// TODO ctor
#endif // GUARD_KUBES_GROUND_OBJECT_STATE_H
