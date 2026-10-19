//
// Created by kubes on 10/14/26.
//
#ifndef GUARD_KUBES_SAVEBLOCK1STATE_H
#define GUARD_KUBES_SAVEBLOCK1STATE_H
#include "mygame/monster_crossing/save_data/outdoor_ground_object_slot.h"
#include "mygame/patches/overworld/map_transition_patches.h"

#define DEBUG_MAP_X 60
#define DEBUG_MAP_Y 50
#define DEBUG_MAP_TILE_COUNT (DEBUG_MAP_X * DEBUG_MAP_Y) // TODO extract map dims

struct PACKED SaveBlock1State
{
    struct OutdoorGroundObjectSlot outdoorGroundObjectSlots[DEBUG_MAP_TILE_COUNT];
    enum OverworldResumeMapContext overworldResumeMapContext;
};
void SaveBlock1State_Create(struct SaveBlock1State* saveBlock1State);
void SaveBlock1State_Clear(struct SaveBlock1State* saveBlock1State);
struct SaveBlock1State* GetSaveBlock1State();
struct OutdoorGroundObjectSlot* GetOutdoorGroundObject(struct SaveBlock1State* saveBlock1State, u8 x, u8 y);

#endif // GUARD_KUBES_SAVEBLOCK1STATE_H
