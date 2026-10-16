//
// Created by kubes on 10/15/26.
//
#include "mygame/monster_crossing/save_data/save_block_1_state.h"

#include "string.h"


static void GenerateSomeFruitOnTheGround(struct SaveBlock1State* saveBlock1State)
{
    const u8 count = 5;
    for (u8 i = 0; i < count; i++)
    {
        struct OutdoorGroundObjectSlot* outdoorGroundObject = GetOutdoorGroundObject(saveBlock1State, 3 + i, 4);
        outdoorGroundObject->objectState.x = GROUND_OBJECT_TYPE_FRUIT;
    }
}

void SaveBlock1State_Create(struct SaveBlock1State* saveBlock1State)
{
    SaveBlock1State_Clear(saveBlock1State);

    GenerateSomeFruitOnTheGround(saveBlock1State);
}

void SaveBlock1State_Clear(struct SaveBlock1State* saveBlock1State)
{
    saveBlock1State->overworldResumeMapContext = RESUME_MAP_CONTEXT_UNINITIALIZED;
    memset(&saveBlock1State->outdoorGroundObjectSlots, 0, sizeof(saveBlock1State->outdoorGroundObjectSlots));
}

struct OutdoorGroundObjectSlot* GetOutdoorGroundObject(struct SaveBlock1State* saveBlock1State, const u8 x, const u8 y)
{
    return &saveBlock1State->outdoorGroundObjectSlots[y * DEBUG_MAP_X + x];
}
