//
// Created by kubes on 10/4/26.
//
#include "mygame/persistent/town_dungeon/cave_data.h"

#include <string.h>

#include "assertf.h"

struct DummyPickupDescription PickupDescription_Create(const u8 isTaken, const u16 itemEnum, const u8 quantity, const s16 x, const s16 y, const u16 objectEventGraphicsEnum)
{
    return (struct DummyPickupDescription) {
        .isTaken = isTaken,
        .itemEnum = itemEnum,
        .quantity = quantity,
        .x = x,
        .y = y,
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

struct DummyDungeonCellData DungeonCellData_Create(const u16 caveEntryCellMapEnum, const s8 caveEntryCellPickupCount, const struct DummyPickupDescription dummyPickupDescriptionArr[MAX_PICKUPS_PER_DUNGEON])
{
    struct DummyDungeonCellData result = {
        .cellMapEnum = caveEntryCellMapEnum,
        .cellPickupDefinitionCount = caveEntryCellPickupCount,
    };
    memcpy(&result.dummyPickupDescription, dummyPickupDescriptionArr, sizeof(result.dummyPickupDescription));
    memset(&result.connections, 0, sizeof(result.connections));

    return result;
}

struct CaveData CaveData_Create()
{
    struct CaveData returnValue;
    memset(&returnValue.dungeonCellsData, 0, sizeof(returnValue.dungeonCellsData)); // redundant todo remove when root data struct is no longer zeroed out on new game
    returnValue.dungeonCellMaxIndex = 4 - 1;
    assertf(returnValue.dungeonCellMaxIndex < MAX_DUNGEON_CELL_COUNT){}
    returnValue.objectHoldingGateKeyStaticIndex = 0;
    return returnValue;
}