//
// Created by kubes on 9/21/26.
//
#ifndef GUARD_KUBES_TESTCALLNATIVE_H
#define GUARD_KUBES_TESTCALLNATIVE_H
#include "global.h"
#include "random.h"

void RegenerateCurrentTownDungeonConfig(void)
{
    struct TownDungeonPersistentData* a = GetCurrentTownDungeonData();
    // todo extract, call here
    a->dummyDungeonData = Random();
    a->dummyTownData = Random();
}
#endif // GUARD_KUBES_TESTCALLNATIVE_H
