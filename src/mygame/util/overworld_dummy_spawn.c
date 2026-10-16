//
// Created by kubes on 10/16/26.
//
#include "mygame/util/overworld_dummy_spawn.h"

#include "global.h"

#include "event_object_movement.h"
#include "constants/event_objects.h"
#include "constants/event_object_movement.h"
#include "constants/trainer_types.h"

static struct ObjectEventTemplate CreateDummyObjectTemplate(const s16 x, const s16 y, const u16 graphicsId)
{
    return (struct ObjectEventTemplate) {
        .x = x,
        .y = y,
        .graphicsId = graphicsId,
        .localId = OBJECT_EVENTS_COUNT, // is this needed?
        .kind = OBJ_KIND_NORMAL,
        .elevation = ELEVATION_DEFAULT,
        .movementType = MOVEMENT_TYPE_NONE,
        .trainerType = TRAINER_TYPE_NONE,
    };
}

struct ObjectEvent* SpawnDummyObject(const s16 x, const s16 y, const u16 graphicsId)
{
    struct ObjectEventTemplate template = CreateDummyObjectTemplate(x, y, graphicsId);

    const u8 objectEventIndex = SpawnSpecialObjectEvent(&template);
    fatal_assertf(objectEventIndex < OBJECT_EVENTS_COUNT, "\n object index of %d exceeds max of %d.", objectEventIndex, OBJECT_EVENTS_COUNT - 1);

    struct ObjectEvent* objectEvent = &gObjectEvents[objectEventIndex];
    objectEvent->localId = OBJECT_EVENTS_COUNT + 1; // is this needed?

    return objectEvent;
}
