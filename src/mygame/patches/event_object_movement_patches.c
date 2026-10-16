//
// Created by kubes on 10/10/26.
//
#include "mygame/patches/event_object_movement_patches.h"

bool8 GetObjectObjectCollidesWith_OWECollisionBegin(struct ObjectEvent* objectEvent, s16 x, s16 y, bool32 addCoords, struct ObjectEvent* otherObject)
{
    return FALSE;
}

bool8 ObjectDespawnsOutsideViewPatch(const struct ObjectEvent* objectEvent)
{
    return TRUE;
}

void OnObjectRemovedOutsideView(struct ObjectEvent* objectEvent)
{

}

void OnOverworldViewPositionForced(struct Coords16 position)
{

}

void OnOverworldViewPositionMoved(struct Coords16 from, struct Coords16 to)
{

}
