//
// Created by kubes on 10/4/26.
//
#include "mygame/persistent/town_dungeon/cave_data.h"

#include <string.h>

#include "assertf.h"
#include "constants/species.h"
#include "mygame/persistent/town_dungeon_persistent_data.h"

struct DummyPickupDescription PickupDescription_Create(const u16 itemEnum, const u16 objectEventGraphicsEnum)
{
    return (struct DummyPickupDescription) {
        .itemEnum = itemEnum,
        .objectEventGraphicsEnum = objectEventGraphicsEnum,
    };
}

struct OverworldMonsterDescription OverworldMonsterDescription_Create(const enum Species monSpecies)
{
    return (struct OverworldMonsterDescription) {
        .monSpecies = monSpecies,
    };
}

struct DungeonCellConnection DungeonCellConnection_Create(const s8 cellIndex, const u8 warpId)
{
    return (struct DungeonCellConnection) {
        .isValid = TRUE,
        .isTown = cellIndex < 0,
        .cellIndex = cellIndex,
        .warpId = warpId,
    };
}

struct DungeonCellData DungeonCellData_Create(const u16 cellMapEnum, const s8 cellPickupDefinitionCount, const s8 cellMonsterDefinitionCount, struct CellPickupConfig pickupConfigs[MAX_PICKUPS_PER_CELL], struct CellOverworldMonsterConfig monsterConfigsArr[MAX_OVERWORLD_MONSTERS_PER_CELL])
{
    struct DungeonCellData result = {
        .cellMapEnum = cellMapEnum,
        .cellPickupDefinitionCount = cellPickupDefinitionCount,
        .cellMonsterDefinitionCount = cellMonsterDefinitionCount,
    };
    memcpy(&result.pickupConfigs, pickupConfigs, sizeof(result.pickupConfigs));
    memcpy(&result.monsterConfigs, monsterConfigsArr, sizeof(result.monsterConfigs));
    memset(&result.connections, 0, sizeof(result.connections));

    return result;
}

struct CaveData CaveData_Create()
{
    struct CaveData returnValue;
    memset(&returnValue.cellsData, 0, sizeof(returnValue.cellsData)); // redundant todo remove when root data struct is no longer zeroed out on new game
    returnValue.maxIndexForCell = 4 - 1;
    assertf(returnValue.maxIndexForCell < MAX_DUNGEON_CELL_COUNT){}
    returnValue.monsterObjectWithGateKeyStaticIndex = 0;
    return returnValue;
}

struct TemporaryCaveState TemporaryCaveState_CreateEmpty()
{
    struct TemporaryCaveState returnValue;
    returnValue.currentCellIndex = 0;
    ClearTemporaryCaveState(&returnValue);
    return returnValue;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
struct TemporaryCaveState* GetTemporaryCaveStatePtr(struct TownDungeonGamePersistentData* this)
{
    _Static_assert(sizeof(struct TemporaryCaveState) <= sizeof(this->temporaryStatePerContext.bytes), "Data size mismatch.");
    fatal_assertf(this->context == CONTEXT_CAVE);
    return (void*) this->temporaryStatePerContext.bytes;
}

struct ObjectIdentifier ObjectIdentifier_Create(const enum ObjectIdentifierType type, const u8 staticIndex)
{
    return (struct ObjectIdentifier) {
        .type = type,
        .staticIndex = staticIndex,
    };
}

void ClearTemporaryCaveState(struct TemporaryCaveState* this)
{
    memset(&this->objectIdByInstanceIndex, 0, sizeof(this->objectIdByInstanceIndex));
}

struct DummyPickupDescription* GetPickupDescription(struct TownDungeonGamePersistentData* gameData, const struct DungeonCellData* cellData, const u8 index)
{
    return &gameData->sharedCavePickupDescriptions[cellData->pickupConfigs[index].sharedDescriptionIndex];
}

bool8* PickupIsTakenPtr(struct DungeonCellData* cellData, const u8 staticIndex)
{
    return &cellData->pickupConfigs[staticIndex].isTaken;
}