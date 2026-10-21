//
// Created by kubes on 10/10/26.
//
#include "mygame/patches/event_object_movement_patches.h"

#include "mygame/persistent/town_dungeon_persistent_data.h"

bool8 GetObjectObjectCollidesWith_OWECollisionBegin(struct ObjectEvent* objectEvent, s16 x, s16 y, bool32 addCoords, struct ObjectEvent* otherObject)
{
    OnGetObjectObjectCollidesWith_OWECollisionBegin(objectEvent, x, y, addCoords, otherObject);

    return FALSE;
}

bool8 ObjectDespawnsOutsideViewPatch(const struct ObjectEvent* objectEvent)
{
    return GetTownDungeonGamePersistentData()->context != CONTEXT_CAVE;

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

// return TRUE to consume/skip TrySpawnObjectEvents call
bool8 TrySpawnObjectEventsPatch_FnBegin(const s16 cameraDeltaX, const s16 cameraDeltaY) {


    return FALSE;
}
