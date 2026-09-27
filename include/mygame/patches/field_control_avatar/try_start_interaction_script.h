//
// Created by kubes on 9/27/26.
//
#ifndef GUARD_KUBES_TRY_START_INTERACTION_SCRIPT_H
#define GUARD_KUBES_TRY_START_INTERACTION_SCRIPT_H
#include "global.h"

u8 GetInteractedObjectEventId(const struct MapPosition* position, u8 metatileBehavior, enum Direction direction);

// returns TRUE when intercepted
u8 TryStartInteractionScript_FnBegin(u8 objectEventId);

#endif // GUARD_KUBES_TRY_START_INTERACTION_SCRIPT_H
