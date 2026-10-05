//
// Created by kubes on 10/5/26.
//
#include "mygame/patches/dynamic_warp/town_dungeon_warps.h"

#include "global.h"
#include "overworld.h"
#include "constants/maps.h"
#include "mygame/persistent/town_dungeon_persistent_data.h"

static struct DungeonCellConnection GetDungeonWarpDestination(const u8 enteredWarpId)
{
    const struct TownDungeonGamePersistentData* gameData = GetTownDungeonGamePersistentData();
    const struct TownDungeonPersistentData* townData = GetCurrentTownDungeonData();
    const struct CaveData* caveData = &townData->caveData;

    const struct DummyDungeonCellData* currentCellData = &caveData->cellsData[gameData->currentCellIndex];
    return currentCellData->connections[enteredWarpId];
}

void SetDynamicWarpFromDungeonCellWarp(const u8 enteredWarpId)
{
    const struct TownDungeonPersistentData* currentTownDungeonData = GetCurrentTownDungeonData();
    const struct CaveData* caveData = &currentTownDungeonData->caveData;

    const struct DungeonCellConnection data = GetDungeonWarpDestination(enteredWarpId);
    // TODO don't use literals for town enum and destinationWarpId
    const u16 destinationMapEnum = data.isTown ? MAP_CAVE_TOWN_00 : caveData->cellsData[data.cellIndex].cellMapEnum;
    const s8 destinationWarpId   = data.isTown ? 0 : (s8) data.warpId;
    GetTownDungeonGamePersistentData()->currentCellIndex = data.isTown ? 0 : data.cellIndex;

    SetDynamicWarp(0, MAP_GROUP(destinationMapEnum), MAP_NUM(destinationMapEnum), destinationWarpId);
}

void SetDynamicWarpFromDungeonTownWarp([[maybe_unused]] u8 enteredWarpId)
{
    struct TownDungeonGamePersistentData* gameData = GetTownDungeonGamePersistentData();
    const struct TownDungeonPersistentData* currentTownData = GetCurrentTownDungeonData();
    const struct CaveData* caveData = &currentTownData->caveData;

    // todo ASSUMES that we always enter into first cell of a dungeon // extract behavior
    gameData->currentCellIndex = 0;
    const struct DummyDungeonCellData destinationCellData = caveData->cellsData[gameData->currentCellIndex];

    const u16 caveEntryCellMapEnum = destinationCellData.cellMapEnum; // todo use getter
    const s8 caveEntranceCellWarpId = currentTownData->caveEntryCellMapWarpId;
    SetDynamicWarp(0, MAP_GROUP(caveEntryCellMapEnum), MAP_NUM(caveEntryCellMapEnum), caveEntranceCellWarpId);
}
