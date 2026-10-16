//
// Created by kubes on 10/14/26.
//
#ifndef GUARD_KUBES_SAVEBLOCK1STATE_H
#define GUARD_KUBES_SAVEBLOCK1STATE_H
#include "mygame/patches/overworld/map_transition_patches.h"

struct SaveBlock1State
{
    enum OverworldResumeMapContext overworldResumeMapContext;
};
void SaveBlock1State_Clear(struct SaveBlock1State* saveBlock1State);

#endif // GUARD_KUBES_SAVEBLOCK1STATE_H
