//
// Created by kubes on 10/10/26.
//
#ifndef GUARD_KUBES_EVENT_OBJECT_MOVEMENT_PATCHES_H
#define GUARD_KUBES_EVENT_OBJECT_MOVEMENT_PATCHES_H
#include "gba/types.h"

struct ObjectEvent;

bool8 OnGetObjectObjectCollidesWith_OWECollisionBegin(struct ObjectEvent *objectEvent, s16 x, s16 y, bool32 addCoords);

#endif // GUARD_KUBES_EVENT_OBJECT_MOVEMENT_PATCHES_H
