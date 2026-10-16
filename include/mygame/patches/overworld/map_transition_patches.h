//
// Created by kubes on 10/15/26.
//
#ifndef GUARD_KUBES_MAP_TRANSITION_PATCHES_H
#define GUARD_KUBES_MAP_TRANSITION_PATCHES_H

// todo how do i patch this ....
    // a
        // PreMapLoadPatch
        // +
        // PreReturnToFieldPatch
        // +
        // ResumeMapPatch
            // switch on context in impl.
    // b
        // OnMapLoadPatch
        // +
        // OnReturnToFieldPatch
        //
        // PreX/ResumeMapPatch are handled for you
        // global context variable
    // c
        // ResumeMap(context)
        //
        // PreX/ResumeMapPatch are handled for you
            // TODO patch
                // [ ] ReturnToFieldLocal
                // [ ] DoMapLoadLoop
        // global context variable
            // in SaveBlock3 and cleared on NewGameInitData

enum __attribute__ ((packed)) OverworldResumeMapContext
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
