//
// Created by kubes on 10/15/26.
//
#include "mygame/patches/overworld/map_transition_patches.h"

#include "global.h"

#include "assertf.h"

#include "mygame/patches/overworld/map_transition_patches_impl.h"

#ifdef OVERWORLD_RESUME_MAP_CONTEXT
static enum OverworldResumeMapContext* GetResumeMapContext()
{
    return OVERWORLD_RESUME_MAP_CONTEXT;
}
#endif

static void SetContext(const enum OverworldResumeMapContext newContext)
{
#ifdef OVERWORLD_RESUME_MAP_CONTEXT
    enum OverworldResumeMapContext* context = GetResumeMapContext();
    fatal_assertf(newContext == RESUME_MAP_CONTEXT_UNINITIALIZED
                  || *context == RESUME_MAP_CONTEXT_UNINITIALIZED);

    *context = newContext;
#endif
}

static void OnReturnToFieldPatch_PreResumeMap()
{
    SetContext(RESUME_MAP_CONTEXT_RETURN_TO_FIELD);
}

void OnReturnToFieldLocalPatch_PreResumeMap()
{
    OnReturnToFieldPatch_PreResumeMap();
}

void OnReturnToFieldLinkPatch_PreResumeMap()
{
    OnReturnToFieldPatch_PreResumeMap();
}

void OnDoMapLoadLoopPatch_PreResumeMap()
{
    SetContext(RESUME_MAP_CONTEXT_MAP_LOADED);
}

static void OnResumeMapPatch(const enum OverworldResumeMapContext context)
{
    OnResumeMapPatch_MyGame(context);
}

void OnResumeMapPatch_FnEnd()
{
#ifdef OVERWORLD_RESUME_MAP_CONTEXT
    const enum OverworldResumeMapContext* context = GetResumeMapContext();
    fatal_assertf(*context != RESUME_MAP_CONTEXT_UNINITIALIZED);

    OnResumeMapPatch(*context);

    SetContext(RESUME_MAP_CONTEXT_UNINITIALIZED);
#endif
}