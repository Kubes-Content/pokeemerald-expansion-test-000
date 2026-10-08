//
// Created by kubes on 10/4/26.
//
#include "mygame/persistent/town_dungeon/cave_data.h"

#include <string.h>

#include "assertf.h"
#include "mygame/persistent/town_dungeon_persistent_data.h"

struct DummyPickupDescription PickupDescription_Create(const u16 itemEnum, const u16 objectEventGraphicsEnum)
{
    return (struct DummyPickupDescription) {
        .itemEnum = itemEnum,
        .objectEventGraphicsEnum = objectEventGraphicsEnum,
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

struct DungeonCellData DungeonCellData_Create(const u16 caveEntryCellMapEnum, const s8 caveEntryCellPickupCount, const u8 pickupDescriptionGameDataIndicesArr[MAX_PICKUPS_PER_CELL])
{
    struct DungeonCellData result = {
        .cellMapEnum = caveEntryCellMapEnum,
        .cellPickupDefinitionCount = caveEntryCellPickupCount,
    };
    memcpy(&result.pickupDescriptionGameDataIndices, pickupDescriptionGameDataIndicesArr, sizeof(result.pickupDescriptionGameDataIndices));
    memset(&result.connections, 0, sizeof(result.connections));
    memset(&result.takenPickups, 0, sizeof(result.takenPickups));

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

struct TemporaryCaveState* GetTemporaryCaveStatePtr(struct TownDungeonGamePersistentData* this)
{
    _Static_assert(sizeof(struct TemporaryCaveState) <= sizeof(this->temporaryStatePerContext.bytes), "Data size mismatch.");
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
    return &gameData->sharedCavePickupDescriptions[cellData->pickupDescriptionGameDataIndices[index]];
}

bool8* PickupIsTakenPtr(struct DungeonCellData* cellData, u8 staticIndex)
{
    return &cellData->takenPickups[staticIndex];
}