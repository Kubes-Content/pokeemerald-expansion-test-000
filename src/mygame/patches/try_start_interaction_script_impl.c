//
// Created by kubes on 10/19/26.
//
#include "mygame/patches/field_control_avatar/try_start_interaction_script_impl.h"

#include "global.h"

#include "event_object_movement.h"
#include "fieldmap.h"
#include "script.h"
#include "task.h"

void UnlockPlayerTask(u8 taskId)
{
    UnlockPlayerFieldControls(); // TODO task steps and cool animations and stuff

    DestroyTask(taskId);
}

bool8 OnTryStartInteractionScript_FnBegin(const u8 objectEventId)
{
    // TODO if a spawned pickup, text box
        // find a better solution than last time, didn't feel very modular
        // TODO when a ground object is spawned, set a temp value relative to its instance index to TRUE
            // WAIT WHY WOULD WE NEED A TEMP VALUE? just take the object event's position and compare that to data (minus offsets, so wrap with a function)
            // TODO set to FALSE for any unloaded object (no need to filter on despawn since its for ALL indices)

    struct ObjectEvent* objectEvent = &gObjectEvents[objectEventId];
    
    struct Coords16 objectCoords = objectEvent->currentCoords;
        objectCoords.x -= MAP_OFFSET;
        objectCoords.y -= MAP_OFFSET;
    assertf(max(objectCoords.x, objectCoords.y) < 256);

    if (objectCoords.x >= DEBUG_MAP_X || objectCoords.y >= DEBUG_MAP_Y) return FALSE;

    struct OutdoorGroundObjectSlot* groundObjectSlot = GetOutdoorGroundObject(GetSaveBlock1State(), objectCoords.x, objectCoords.y);
    if (groundObjectSlot->objectState.groundObjectType != GROUND_OBJECT_TYPE_FRUIT) return FALSE; // TODO create function to handle per object type

    groundObjectSlot->objectState.groundObjectType = GROUND_OBJECT_TYPE_UNINITIALIZED;
    RemoveObjectEvent(objectEvent);

    CreateTask(UnlockPlayerTask, 0);

    return TRUE;
}
