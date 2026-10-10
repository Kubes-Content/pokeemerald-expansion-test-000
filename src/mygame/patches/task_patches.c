//
// Created by kubes on 10/10/26.
//
#include "mygame/patches/task_patches.h"

#include "mygame/persistent/town_dungeon_persistent_data.h"

void RunTasks_FnBegin(u8 firstActiveTaskId)
{
    OnRunTasks_FnBegin(firstActiveTaskId);
}
