//
// Created by kubes on 10/15/26.
//
#include "mygame/monster_crossing/save_data/save_block_1_state.h"

#include "global.h"

#include "string.h"


static void GenerateSomeFruitOnTheGround(struct SaveBlock1State* saveBlock1State)
{
    const u8 count = 64;
    const u8 rows = 5;
    const u8 xOffset = 20;
    const u8 yOffset = 16;
    for (u8 i = 0; i < count; i++)
    {
        struct OutdoorGroundObjectSlot* outdoorGroundObject = GetOutdoorGroundObject(saveBlock1State, xOffset + i / rows, yOffset + i % rows);
        outdoorGroundObject->objectState.groundObjectType = GROUND_OBJECT_TYPE_FRUIT;
    }
}

void SaveBlock1State_Create(struct SaveBlock1State* saveBlock1State)
{
    SaveBlock1State_Clear(saveBlock1State);

    //GenerateSomeFruitOnTheGround(saveBlock1State);
}

void SaveBlock1State_Clear(struct SaveBlock1State* saveBlock1State)
{
    saveBlock1State->overworldResumeMapContext = RESUME_MAP_CONTEXT_UNINITIALIZED;
    memset(&saveBlock1State->outdoorGroundObjectSlots, 0, sizeof(saveBlock1State->outdoorGroundObjectSlots));
}

struct SaveBlock1State* GetSaveBlock1State()
{
    return &gSaveBlock1Ptr->myGameSaveBlock1State;
}

struct OutdoorGroundObjectSlot* GetOutdoorGroundObject(struct SaveBlock1State* saveBlock1State, const u8 x, const u8 y)
{
    return &saveBlock1State->outdoorGroundObjectSlots[y * DEBUG_MAP_X + x];
}
