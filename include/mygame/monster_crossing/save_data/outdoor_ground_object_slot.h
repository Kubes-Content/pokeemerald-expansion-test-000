//
// Created by kubes on 10/16/26.
//
#ifndef GUARD_KUBES_OUTDOOR_GROUND_OBJECT_SLOT_H
#define GUARD_KUBES_OUTDOOR_GROUND_OBJECT_SLOT_H
#include "gba/types.h"
#include "mygame/monster_crossing/ground_object_state.h"

struct PACKED OutdoorGroundObjectSlot
{
    struct GroundObjectState objectState;
};
// TODO to .c
/*bool8 IsBuried(const struct OutdoorGroundObjectSlot* slot) { return slot->objectState.variant; }
void SetBuried(struct OutdoorGroundObjectSlot* slot, bool8 value) { slot->objectState.variant = value; }*/
inline struct OutdoorGroundObjectSlot OutdoorGroundObjectSlot_Create(const struct GroundObjectState objectState, const bool8 buried)
{
    struct OutdoorGroundObjectSlot returnValue = (struct OutdoorGroundObjectSlot) {
        .objectState = objectState
    };
    // TODO SetBuried(&returnValue, buried);

    return returnValue;
}

#endif // GUARD_KUBES_OUTDOOR_GROUND_OBJECT_SLOT_H
