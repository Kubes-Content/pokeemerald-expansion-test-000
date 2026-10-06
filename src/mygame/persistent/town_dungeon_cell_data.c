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

struct DummyDungeonCellData DungeonCellData_Create(const u16 caveEntryCellMapEnum, const s8 caveEntryCellPickupCount, const u8 dummyPickupDescriptionArr[NUM_PICKUP_DESCRIPTIONS_PER_CELL])
{
    struct DummyDungeonCellData result = {
        .cellMapEnum = caveEntryCellMapEnum,
        .cellPickupDefinitionCount = caveEntryCellPickupCount,
    };
    memcpy(&result.relevantPickupIndices, dummyPickupDescriptionArr, sizeof(result.relevantPickupIndices));
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
    returnValue.objectWithGateKeyStaticIndex = 0;
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

void ClearTemporaryCaveState(struct TemporaryCaveState* this)
{
    // TODO vvv why 1? vvv
    memset(&this->objectStaticPickupIndexByInstanceIndex, 1, sizeof(this->objectStaticPickupIndexByInstanceIndex));
}

struct DummyPickupDescription* GetPickupDescription(struct TownDungeonGamePersistentData* gameData, const struct DummyDungeonCellData* cellData, const u8 index)
{
    return &gameData->sharedCavePickupDescriptions[cellData->relevantPickupIndices[index]];
}

bool8* PickupIsTakenPtr(struct DummyDungeonCellData* cellData, u8 staticIndex)
{
    return &cellData->takenPickups[staticIndex];
}