//
// Created by kubes on 10/15/26.
//
#include "mygame/patches/new_game/new_game_init_data_impl.h"

#include "global.h"
#include "mygame/monster_crossing/save_data/save_block_1_state.h"

void OnNewGameInitDataPatch_FnEnd()
{
    struct SaveBlock1State* myGameSaveBlock1State = &gSaveBlock1Ptr->myGameSaveBlock1State; // todo extract to function
    SaveBlock1State_Clear(myGameSaveBlock1State);
}
