//
// Created by kubes on 10/4/26.
//
#ifndef GUARD_KUBES_TOWN_DUNGEON_CELL_DATA_H
#define GUARD_KUBES_TOWN_DUNGEON_CELL_DATA_H
#include "metaprogram.h"
#include "gba/types.h"
#include "constants/items.h"
#include "constants/event_objects.h"
#include "constants/species.h"
#include "mygame/patches/global/constants.h"
#include "mygame/persistent/town_dungeon/cave/cell_ow_monster_data.h"

#define MAX_DUNGEON_CELL_COUNT 8
#define MAX_PICKUPS_PER_CELL 8
#define MAX_OVERWORLD_MONSTERS_PER_CELL 8
#define MAX_DUNGEON_CELL_CONNECTIONS_PER_CELL 4

struct TownDungeonGamePersistentData;

struct DummyPickupDescription
{
    u16 itemEnum : BIT_SIZE(ITEMS_COUNT - 1); // should be able to give nothing so that I can use a pickup like a toggle or obstacle
    u16 objectEventGraphicsEnum : BIT_SIZE(NUM_OBJ_EVENT_GFX - 1);
};
struct DummyPickupDescription PickupDescription_Create(u16 itemEnum, u16 objectEventGraphicsEnum);

struct OverworldMonsterDescription
{
    enum Species monSpecies : BIT_SIZE(NUM_SPECIES - 1);
    u16 objectEventGraphicsEnum : BIT_SIZE(NUM_OBJ_EVENT_GFX - 1);
};
struct OverworldMonsterDescription OverworldMonsterDescription_Create(enum Species monSpecies, u16 objectEventGraphicsEnum);

struct DungeonCellConnection
{
    bool8 isValid : 1;
    bool8 isTown  : 1;
    u8 cellIndex  : BIT_SIZE(MAX_DUNGEON_CELL_COUNT - 1);
    u8    warpId  : BIT_SIZE(127);
};
struct DungeonCellConnection DungeonCellConnection_Create(s8 cellIndex, u8 warpId);

struct DungeonCellData
{
    u16 cellMapEnum : BIT_SIZE(MAP_COUNT - 1); // todo wrap in struct CellDescription
    u8 cellPickupDefinitionCount : BIT_SIZE(MAX_PICKUPS_PER_CELL - 1); // todo shouldn't be minus one....
    u8 cellMonsterDefinitionCount : BIT_SIZE(MAX_OVERWORLD_MONSTERS_PER_CELL);
    u8 takenPickups[MAX_PICKUPS_PER_CELL];
    u8 pickupDescriptionGameDataIndices[MAX_PICKUPS_PER_CELL];
    struct CellOverworldMonsterConfig monsterConfigs[MAX_OVERWORLD_MONSTERS_PER_CELL];
    struct DungeonCellConnection connections[MAX_DUNGEON_CELL_CONNECTIONS_PER_CELL];
};
struct DungeonCellData DungeonCellData_Create(u16 cellMapEnum, s8 cellPickupDefinitionCount, s8 cellMonsterDefinitionCount, const u8 pickupDescriptionGameDataIndicesArr[MAX_PICKUPS_PER_CELL], struct CellOverworldMonsterConfig monsterConfigsArr[MAX_OVERWORLD_MONSTERS_PER_CELL]);

struct CaveData
{
    u8 maxIndexForCell : BIT_SIZE(MAX_DUNGEON_CELL_COUNT - 1); // cell ct - 1
    u8 monsterObjectWithGateKeyStaticIndex : BIT_SIZE(OBJECT_EVENTS_COUNT - 1);
    struct DungeonCellData cellsData[MAX_DUNGEON_CELL_COUNT];
};
struct CaveData CaveData_Create();

enum ObjectIdentifierType
{
    OBJ_ID_DEFAULT,
    OBJ_ID_NOTHING,
    OBJ_ID_PICKUP,
    OBJ_ID_MONSTER,
    OBJECT_IDENTIFIER_TYPE_COUNT
};

struct ObjectIdentifier
{
    enum ObjectIdentifierType type: BIT_SIZE(OBJECT_IDENTIFIER_TYPE_COUNT);
    u8 staticIndex: BIT_SIZE(OBJECT_EVENTS_COUNT);
};
struct ObjectIdentifier ObjectIdentifier_Create(enum ObjectIdentifierType type, u8 staticIndex);

// savable data that mostly just represents the cell you're currently in (regenerates on entering cell/cave)
struct TemporaryCaveState
{
    u8 currentCellIndex : BIT_SIZE(MAX_DUNGEON_CELL_COUNT - 1);
    struct ObjectIdentifier objectIdByInstanceIndex[OBJECT_EVENTS_COUNT]; // TODO init with cave state, zeroed out should be fine
    // TODO how do we differentiate pickups/NPCs/interactable when player interacts?
        // so any object has the same runtime state
        // 1. enum of its class (pickup, monster, other)
        // 2. index value dependent on that class
            // pickups use the index to get their index into the root pickup desc. array
            // NPCs use the index to get their index into the root NPC desc. array
                // to allow variable behavior (monsters vs. trainers vs. civilians vs. moving obstacle)
        // standardize connection of pickups and NPCs to their root desc. array
            // so the pattern's easy to repeat
    // clr instance on destroy
};
struct TemporaryCaveState TemporaryCaveState_CreateEmpty();
struct TemporaryCaveState* GetTemporaryCaveStatePtr(struct TownDungeonGamePersistentData* this);
void ClearTemporaryCaveState(struct TemporaryCaveState* this);

struct DummyPickupDescription* GetPickupDescription(struct TownDungeonGamePersistentData* gameData, const struct DungeonCellData* cellData, u8 index);

bool8* PickupIsTakenPtr(struct DungeonCellData* cellData, u8 staticIndex);

#endif // GUARD_KUBES_TOWN_DUNGEON_CELL_DATA_H
