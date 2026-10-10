#include "global.h"

#include "event_object_movement.h"
#include "global.fieldmap.h"
#include "task.h"
#include "constants/event_objects.h"
#include "constants/trainer_types.h"
#include "mygame/persistent/town_dungeon_persistent_data.h"
#include "mygame/util/MapHeader.h"

// todo extract
static u8 SpawnBaseObject(s16 x, s16 y, u16 graphicsId)
{
    struct ObjectEventTemplate template = {
        .x = x,
        .y = y,
        .graphicsId = graphicsId,
        .localId = OBJECT_EVENTS_COUNT,
        .kind = OBJ_KIND_NORMAL,
        .elevation = ELEVATION_DEFAULT,
        .movementType = MOVEMENT_TYPE_NONE,
        .trainerType = TRAINER_TYPE_NONE,
    };

    const u8 objectEventIndex = SpawnSpecialObjectEvent(&template);

    fatal_assertf(objectEventIndex < OBJECT_EVENTS_COUNT, "\n object index of %d exceeds max of %d.", objectEventIndex, OBJECT_EVENTS_COUNT - 1);
    const u8 idThatPreventsUnloadingWhileOffscreen = OBJ_EVENT_ID_NPC_FOLLOWER; // todo extract
    gObjectEvents[objectEventIndex].localId = idThatPreventsUnloadingWhileOffscreen;

    return objectEventIndex;
}

// todo refactor
static u8 SpawnDungeonPickup(const u8 staticIndex, struct TownDungeonGamePersistentData* gameData, struct TemporaryCaveState* temporaryCaveState, const struct DungeonCellData* cellData)
{
    const struct DummyPickupDescription* pickupDescription = GetPickupDescription(gameData, cellData, staticIndex);
    const struct CellPickupConfig* cellPickupConfig = &cellData->pickupConfigs[staticIndex];
    const u8 instanceIndex = SpawnBaseObject(cellPickupConfig->x, cellPickupConfig->y, pickupDescription->objectEventGraphicsEnum);//SpawnLocalClone(LOCALID_DYNAMIC_INTERACTABLE_TEMPLATE, pickupDescription->x, pickupDescription->y, pickupDescription->objectEventGraphicsEnum);

    struct ObjectIdentifier* objectIdentifier = &temporaryCaveState->objectIdByInstanceIndex[instanceIndex];
    *objectIdentifier = ObjectIdentifier_Create(OBJ_ID_PICKUP, staticIndex);

    return instanceIndex;
}

static u8 GetUnusedWarps(const struct DungeonCellData* cellData, struct WarpEvent* unusedWarpsArr, u8 arrLength)
{
    struct WarpEvent allWarpsArr[arrLength];
    const u8 warpCount = GetAllWarps(cellData->cellMapEnum, allWarpsArr, arrLength);

    u8 unusedWarpsCount = 0;

    for (u8 i = 0; i < warpCount; i++)
    {
        if (cellData->connections[i].isValid)
            continue;

        unusedWarpsArr[unusedWarpsCount] = allWarpsArr[i];
        unusedWarpsCount++;
    }

    return unusedWarpsCount;
}

static void SpawnGarbageOverUnusedDoors(const struct DungeonCellData* cellData)
{
    // ASSUMING THAT LOCAL WARP ID IS CONNECTIONS INDEX

    const u8 unusedDoorsCapacity = 4; // TODO this is gonna be a problem...
    struct WarpEvent unusedWarps[unusedDoorsCapacity];
    const u8 unusedDoorsCount = GetUnusedWarps(cellData, unusedWarps, unusedDoorsCapacity);

    // spawn pickups over unused warps
    for (u8 i = 0; i < unusedDoorsCount; i++)
    {
        const u8 x = unusedWarps[i].x;
        const u8 y = unusedWarps[i].y;
        [[maybe_unused]] const u8 instanceIndex = SpawnBaseObject(x, y, OBJ_EVENT_GFX_MOVING_BOX);
    }
}

static void SpawnPickups(struct TownDungeonGamePersistentData* gameData, const struct TownDungeonPersistentData* this, struct TemporaryCaveState* temporaryCaveState, struct DungeonCellData* cellData)
{
    for (u8 staticIndex = 0; staticIndex < cellData->cellPickupDefinitionCount; staticIndex++)
    {
        if (!*PickupIsTakenPtr(cellData, staticIndex))
        {
            SpawnDungeonPickup(staticIndex, gameData, temporaryCaveState, cellData);
        }
    }

    SpawnGarbageOverUnusedDoors(cellData);
}

[[nodiscard]]
static u8 SpawnMonsterObject(struct TownDungeonGamePersistentData* gameData,
                             struct TemporaryCaveState* temporaryCaveState,
                             struct TownDungeonPersistentData* currentTownData,
                             struct DungeonCellData* cellData,
                             const u8 staticIndex)
{
    struct CellOverworldMonsterConfig* cellOverworldMonsterConfig = &cellData->monsterConfigs[staticIndex];
    struct OverworldMonsterDescription* sharedMonsterDescription = &gameData->sharedMonsterDescriptions[cellOverworldMonsterConfig->sharedDescriptionIndex];
    const u16 graphicsId = sharedMonsterDescription->objectEventGraphicsEnum;
    const u8 instanceIndex = SpawnBaseObject(cellOverworldMonsterConfig->homeX, cellOverworldMonsterConfig->homeY, graphicsId);

    struct ObjectIdentifier* objectIdentifier = &temporaryCaveState->objectIdByInstanceIndex[instanceIndex];
    *objectIdentifier = ObjectIdentifier_Create(OBJ_ID_MONSTER, staticIndex);

    return instanceIndex;
}

static void SpawnMonsters(struct TownDungeonGamePersistentData* gameData,
                          struct TemporaryCaveState* temporaryCaveState,
                          struct TownDungeonPersistentData* currentTownData,
                          struct DungeonCellData* cellData)
{
    for (u8 i = 0; i < cellData->cellMonsterDefinitionCount; i++)
    {
        [[maybe_unused]] const u8 instanceIndex = SpawnMonsterObject(gameData, temporaryCaveState, currentTownData, cellData, i);
    }

}

static void TickCaveMonster(struct TownDungeonGamePersistentData* gameData,
                            struct TemporaryCaveState* temporaryCaveState, struct
                            CellOverworldMonsterConfig* cellConfig, u8 objectInstanceIndex)
{
    if (cellConfig->isDead) return;

    struct ObjectEvent* objectEvent = &gObjectEvents[objectInstanceIndex];

    if (ObjectEventIsHeldMovementActive(objectEvent) && !ObjectEventClearHeldMovementIfFinished(objectEvent)) return;

    // TODO get movement behavior from description

    const u8 direction = DIR_SOUTH;
    const u8 collisionInDirection = GetCollisionInDirection(objectEvent, direction);
    if (collisionInDirection != COLLISION_NONE) return;

    ObjectEventSetHeldMovement(objectEvent, GetWalkNormalMovementAction(direction));
}

static void TestRootTaskTask(u8 taskId)
{
    //u8* data = (void*) gTasks[taskId].data;

    struct TownDungeonGamePersistentData* gameData = GetTownDungeonGamePersistentData();
    struct TemporaryCaveState* temporaryCaveState = GetTemporaryCaveStatePtr(gameData);
    struct TownDungeonPersistentData* currentTownData = GetCurrentTownDungeonData();
    struct CaveData* caveData = &currentTownData->caveData;
    struct DungeonCellData* cellData = &caveData->cellsData[temporaryCaveState->currentCellIndex];

    const u8 objectIdByInstanceIndexLength = sizeof(temporaryCaveState->objectIdByInstanceIndex) / sizeof(temporaryCaveState->objectIdByInstanceIndex[0]);
    for (u8 objectInstanceIndex = 0; objectInstanceIndex < objectIdByInstanceIndexLength; objectInstanceIndex++)
    {
        const struct ObjectIdentifier* objectIdentifier = &temporaryCaveState->objectIdByInstanceIndex[objectInstanceIndex];

        const u8 staticIndex = objectIdentifier->staticIndex;
        switch (objectIdentifier->type)
        {
        case OBJ_ID_DEFAULT:
        case OBJ_ID_NOTHING:
        case OBJ_ID_PICKUP:
            break;
        case OBJ_ID_MONSTER:;
            struct CellOverworldMonsterConfig* cellOverworldMonsterConfig = &cellData->monsterConfigs[staticIndex];
            TickCaveMonster(gameData, temporaryCaveState, cellOverworldMonsterConfig, objectInstanceIndex);
            break;
        case OBJECT_IDENTIFIER_TYPE_COUNT:
            fatalf();
        }
    }
}

// ReSharper disable once CppUseInternalLinkage
void OnDungeonCellLoaded()
{
    struct TownDungeonGamePersistentData* gameData = GetTownDungeonGamePersistentData();
    struct TemporaryCaveState* temporaryCaveState = GetTemporaryCaveStatePtr(gameData);
    struct TownDungeonPersistentData* currentTownData = GetCurrentTownDungeonData();
    struct CaveData* caveData = &currentTownData->caveData;
    struct DungeonCellData* cellData = &caveData->cellsData[temporaryCaveState->currentCellIndex];

    fatal_assertf(!RootTaskIsRunning(gameData));
    const u8 rootTaskId = SetRootTask(gameData, TestRootTaskTask);
    u8* rootTaskDataArr = (void*) gTasks[rootTaskId].data;

    gameData->context = CONTEXT_CAVE;
    ClearTemporaryCaveState(temporaryCaveState);
    SpawnPickups(gameData, currentTownData, temporaryCaveState, cellData);
    SpawnMonsters(gameData, temporaryCaveState, currentTownData, cellData);
}