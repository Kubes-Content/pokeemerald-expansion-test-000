#include "global.h"

#include "event_object_movement.h"
#include "global.fieldmap.h"
#include "constants/event_objects.h"
#include "constants/trainer_types.h"
#include "gba/defines.h"
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

    assertf(objectEventIndex < OBJECT_EVENTS_COUNT, "\n object index of %d exceeds max of %d.", objectEventIndex, OBJECT_EVENTS_COUNT - 1){}
    const u8 idThatPreventsUnloadingWhileOffscreen = OBJ_EVENT_ID_FOLLOWER; // todo extract
    gObjectEvents[objectEventIndex].localId = idThatPreventsUnloadingWhileOffscreen;

    return objectEventIndex;
}

// todo refactor
static u8 SpawnDungeonPickup(struct TownDungeonPersistentData* this, u8 staticIndex)
{
    struct TownDungeonGamePersistentData* gameData = GetTownDungeonGamePersistentData();
    struct TemporaryCaveState* temporaryCaveState = GetTemporaryCaveStatePtr(gameData);
    const struct CaveData* caveData = &GetCurrentTownDungeonData()->caveData;
    const struct DummyDungeonCellData* cellData = &caveData->cellsData[temporaryCaveState->currentCellIndex]; // refactor todo get current cell's index from game data
    const struct DummyPickupDescription* pickupDescription = GetPickupDescription(gameData, cellData, staticIndex);
    // TODO dynamic spawn positions
    const u8 instanceIndex = SpawnBaseObject(5 + staticIndex, 15, pickupDescription->objectEventGraphicsEnum);//SpawnLocalClone(LOCALID_DYNAMIC_INTERACTABLE_TEMPLATE, pickupDescription->x, pickupDescription->y, pickupDescription->objectEventGraphicsEnum);

    struct ObjectIdentifier* objectIdentifier = &temporaryCaveState->objectIdByInstanceIndex[instanceIndex];
    *objectIdentifier = ObjectIdentifier_Create(OBJ_ID_PICKUP, staticIndex);

    return instanceIndex;
}

static u8 GetUnusedWarps(const struct DummyDungeonCellData* cellData, struct WarpEvent* unusedWarpsArr, u8 arrLength)
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

static void SpawnGarbageOverUnusedDoors(struct TownDungeonPersistentData* this)
{
    struct TownDungeonGamePersistentData* gameData = GetTownDungeonGamePersistentData();
    const struct TemporaryCaveState* temporaryCaveState = GetTemporaryCaveStatePtr(gameData);
    const struct CaveData* caveData = &GetCurrentTownDungeonData()->caveData;
    const struct DummyDungeonCellData* cellData = &caveData->cellsData[temporaryCaveState->currentCellIndex];

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

static void SpawnPickups(struct TownDungeonPersistentData* this)
{
    struct CaveData* caveData = &GetCurrentTownDungeonData()->caveData; // todo why not pass this as arg too?
    struct TownDungeonGamePersistentData* gameData = GetTownDungeonGamePersistentData(); // todo why not pass this as arg too?
    const struct TemporaryCaveState* temporaryCaveState = GetTemporaryCaveStatePtr(gameData);
    struct DummyDungeonCellData* cellData = &caveData->cellsData[temporaryCaveState->currentCellIndex];
    // TODO replace 1 w/ MAX_PICKUPS_PER_DUNGEON
    for (u8 staticIndex = 0; staticIndex < cellData->cellPickupDefinitionCount; staticIndex++)
    {
        if (!*PickupIsTakenPtr(cellData, staticIndex))
            SpawnDungeonPickup(this, staticIndex);
    }

    SpawnGarbageOverUnusedDoors(this);
}

// ReSharper disable once CppUseInternalLinkage
void OnDungeonCellLoaded()
{
    struct TownDungeonGamePersistentData* gameData = GetTownDungeonGamePersistentData();
    struct TemporaryCaveState* temporaryCaveState = GetTemporaryCaveStatePtr(gameData);
    struct TownDungeonPersistentData* currentTownData = GetCurrentTownDungeonData();

    ClearTemporaryCaveState(temporaryCaveState);
    gameData->context = CONTEXT_CAVE;
    SpawnPickups(currentTownData);
}