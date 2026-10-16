//
// Created by kubes on 10/15/26.
//
#ifndef GUARD_KUBES_MAP_TRANSITION_PATCHES_H
#define GUARD_KUBES_MAP_TRANSITION_PATCHES_H
#include "gba/defines.h"

enum PACKED OverworldResumeMapContext
{
    RESUME_MAP_CONTEXT_UNINITIALIZED,
    RESUME_MAP_CONTEXT_RETURN_TO_FIELD,
    RESUME_MAP_CONTEXT_MAP_LOADED,
};

void OnReturnToFieldLocalPatch_PreResumeMap(void);
void OnReturnToFieldLinkPatch_PreResumeMap(void);
void OnDoMapLoadLoopPatch_PreResumeMap(void);
void OnResumeMapPatch_FnEnd(void);

#endif // GUARD_KUBES_MAP_TRANSITION_PATCHES_H
