//
// Created by kubes on 10/10/26.
//
#include "mygame/patches/event_object_movement_patches.h"

#include "mygame/persistent/town_dungeon_persistent_data.h"

bool8 GetObjectObjectCollidesWith_OWECollisionBegin(struct ObjectEvent* objectEvent, s16 x, s16 y, bool32 addCoords, struct ObjectEvent* otherObject)
{
    if (OnGetObjectObjectCollidesWith_OWECollisionBegin(objectEvent, x, y, addCoords, otherObject)) return TRUE;

    return FALSE;
}
