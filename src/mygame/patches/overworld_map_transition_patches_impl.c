//
// Created by kubes on 2026-10-15
//
#include "mygame/patches/overworld/map_transition_patches_impl.h"

#include "global.h"

#include "task.h"
#include "constants/event_objects.h"

#include "mygame/monster_crossing/save_data/save_block_1_state.h"
#include "mygame/patches/overworld/map_transition_patches.h"
#include "mygame/util/overworld_dummy_spawn.h"

static void ATask(const u8 taskId)
{
    struct SaveBlock1State* saveBlock1State = &gSaveBlock1Ptr->myGameSaveBlock1State;
    //const struct ObjectEvent* playerObject = &gObjectEvents[gPlayerAvatar.objectEventId];

    DestroyTask(taskId);
}

void OnResumeMapPatch_MyGame(const enum OverworldResumeMapContext context)
{
    if (context != RESUME_MAP_CONTEXT_MAP_LOADED) return;

    //CreateTask(ATask, 0);
}
