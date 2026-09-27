//
// Created by kubes on 9/27/26.
//
#include "mygame/patches/field_control_avatar/try_start_interaction_script.h"

#include "global.h"

#include "event_data.h"
#include "event_object_movement.h"
#include "fieldmap.h"
#include "metatile_behavior.h"
#include "overworld.h"
#include "constants/event_objects.h"

// stolen from field_control_avatar.c:GetInteractedObjectEventScript
u8 GetInteractedObjectEventId(const struct MapPosition* position, const u8 metatileBehavior, const enum Direction direction)
{
    u8 objectEventId;
    const s16 currX = gObjectEvents[gPlayerAvatar.objectEventId].currentCoords.x;
    const s16 currY = gObjectEvents[gPlayerAvatar.objectEventId].currentCoords.y;
    const u8 currBehavior = MapGridGetMetatileBehaviorAt(currX, currY);

    gSpecialVar_Facing = direction;

    switch (direction)
    {
    case DIR_EAST:
        if (MetatileBehavior_IsSidewaysStairsLeftSideAny(metatileBehavior))
            // sideways stairs left-side to your right -> check northeast
            objectEventId = GetObjectEventIdByPosition(currX + 1, currY - 1, position->elevation);
        else if (MetatileBehavior_IsSidewaysStairsRightSideAny(currBehavior))
            // on top of right-side stairs -> check southeast
            objectEventId = GetObjectEventIdByPosition(currX + 1, currY + 1, position->elevation);
        else
            // check in front of player
            objectEventId = GetObjectEventIdByPosition(position->x, position->y, position->elevation);
        break;
    case DIR_WEST:
        if (MetatileBehavior_IsSidewaysStairsRightSideAny(metatileBehavior))
            // facing sideways stairs right side -> check northwest
            objectEventId = GetObjectEventIdByPosition(currX - 1, currY - 1, position->elevation);
        else if (MetatileBehavior_IsSidewaysStairsLeftSideAny(currBehavior))
            // on top of left-side stairs -> check southwest
            objectEventId = GetObjectEventIdByPosition(currX - 1, currY + 1, position->elevation);
        else
            // check in front of player
            objectEventId = GetObjectEventIdByPosition(position->x, position->y, position->elevation);
        break;
    default:
        objectEventId = GetObjectEventIdByPosition(position->x, position->y, position->elevation);
        break;
    }

    if (objectEventId == OBJECT_EVENTS_COUNT || gObjectEvents[objectEventId].localId == LOCALID_PLAYER)
    {
        if (MetatileBehavior_IsCounter(metatileBehavior) != TRUE)
            return OBJECT_EVENTS_COUNT;

        // Look for an object event on the other side of the counter.
        objectEventId = GetObjectEventIdByPosition(position->x + gDirectionToVectors[direction].x, position->y + gDirectionToVectors[direction].y, position->elevation);
        if (objectEventId == OBJECT_EVENTS_COUNT || gObjectEvents[objectEventId].localId == LOCALID_PLAYER)
            return OBJECT_EVENTS_COUNT;
    }

    gSelectedObjectEvent = objectEventId;
    gSpecialVar_LastTalked = gObjectEvents[objectEventId].localId;

    return objectEventId;
}

// returns TRUE when intercepted
u8 TryStartInteractionScript_FnBegin(const u8 objectEventId)
{
    if (objectEventId == OBJECT_EVENTS_COUNT)
        return FALSE;

    return FALSE;
}