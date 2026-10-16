//
// Created by kubes on 10/10/26.
//
#ifndef GUARD_KUBES_EVENT_OBJECT_MOVEMENT_PATCHES_H
#define GUARD_KUBES_EVENT_OBJECT_MOVEMENT_PATCHES_H
#include "global.h"

struct ObjectEvent;

bool8 GetObjectObjectCollidesWith_OWECollisionBegin(struct ObjectEvent *objectEvent, s16 x, s16 y, bool32 addCoords, struct ObjectEvent* otherObject);
bool8 ObjectDespawnsOutsideViewPatch(const struct ObjectEvent* objectEvent);
void OnObjectRemovedOutsideView(struct ObjectEvent* objectEvent);
void OnOverworldViewPositionForced(struct Coords16 position);
void OnOverworldViewPositionMoved(struct Coords16 from, struct Coords16 to);

#endif // GUARD_KUBES_EVENT_OBJECT_MOVEMENT_PATCHES_H
