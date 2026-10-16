//
// Created by kubes on 10/15/26.
//
#include "mygame/monster_crossing/save_data/save_block_1_state.h"

void SaveBlock1State_Clear(struct SaveBlock1State* saveBlock1State)
{
    saveBlock1State->overworldResumeMapContext = RESUME_MAP_CONTEXT_UNINITIALIZED;
}
