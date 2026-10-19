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

static void SpawnSomeFruitTask(const u8 taskId)
{
    struct SaveBlock1State* saveBlock1State = &gSaveBlock1Ptr->myGameSaveBlock1State;
    //const struct ObjectEvent* playerObject = &gObjectEvents[GetPlayerAvatarSpriteId()];

    // TODO limit to initial viewing bounds
    for (u8 x = 0; x < DEBUG_MAP_X; x++) for (u8 y = 0; y < DEBUG_MAP_Y; y++)
    {
        const struct OutdoorGroundObjectSlot* outdoorGroundObject = GetOutdoorGroundObject(saveBlock1State, x, y);

        if (outdoorGroundObject->objectState.groundObjectType == GROUND_OBJECT_TYPE_FRUIT)
            SpawnDummyObject(x, y, OBJ_EVENT_GFX_EXAMPLE);
    }

    DestroyTask(taskId);
}

void OnResumeMapPatch_MyGame(const enum OverworldResumeMapContext context)
{
    if (context != RESUME_MAP_CONTEXT_MAP_LOADED) return;

    //CreateTask(SpawnSomeFruitTask, 0);
}
