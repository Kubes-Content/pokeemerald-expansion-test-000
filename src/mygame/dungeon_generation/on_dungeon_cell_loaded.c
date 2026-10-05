#include "global.h"

#include "event_object_movement.h"
#include "gba/defines.h"
#include "global.fieldmap.h"
#include "overworld.h"
#include "constants/event_objects.h"
#include "constants/trainer_types.h"

#include "mygame/persistent/town_dungeon_persistent_data.h"

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

    if (objectEventIndex != OBJECT_EVENTS_COUNT)
        gObjectEvents[objectEventIndex].localId = OBJ_EVENT_ID_FOLLOWER; // prevents unloading when outside of frame

    return objectEventIndex;
}

static u8 SpawnDungeonPickup(struct TownDungeonPersistentData* this, u8 staticIndex)
{
    struct TownDungeonGamePersistentData* gameData = GetTownDungeonGamePersistentData();
    const struct CaveData* caveData = &GetCurrentTownDungeonData()->caveData;
    const struct DummyDungeonCellData* cellData = &caveData->cellsData[gameData->currentCellIndex]; // refactor todo get current cell's index from game data
    const struct DummyPickupDescription* pickupDescription = GetPickupDescription(gameData, cellData, staticIndex);
    // TODO dynamic spawn positions
    const u8 instanceIndex = SpawnBaseObject(5 + staticIndex, 15, pickupDescription->objectEventGraphicsEnum);//SpawnLocalClone(LOCALID_DYNAMIC_INTERACTABLE_TEMPLATE, pickupDescription->x, pickupDescription->y, pickupDescription->objectEventGraphicsEnum);

    gameData->objectStaticPickupIndexByInstanceIndex[instanceIndex] = staticIndex; // OnInteract will leverage this

    return instanceIndex;
}

static u8 GetUnusedWarps(const struct DummyDungeonCellData* cellData, u8* unusedDoorsXArr, u8* unusedDoorsYArr, u8 arrLength)
{
    const struct MapHeader* const mapHeader = Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(cellData->cellMapEnum), MAP_NUM(cellData->cellMapEnum));
    u8 unusedWarpsCount = 0;

    fatal_assertf(mapHeader->events->warpCount <= arrLength);

    for (u8 i = 0; i < mapHeader->events->warpCount; i++) // todo ctor CellVariantRestrictions
    {
        if (cellData->connections[i].isValid)
            continue;

        /*const s32 width = mapHeader->mapLayout->width;
        const s32 height = mapHeader->mapLayout->height;*/
        const s16 x = mapHeader->events->warps[i].x;
        const s16 y = mapHeader->events->warps[i].y;

        unusedDoorsXArr[unusedWarpsCount] = x;
        unusedDoorsYArr[unusedWarpsCount] = y;
        unusedWarpsCount++;
    }

    return unusedWarpsCount;
}

static void SpawnGarbageOverUnusedDoors(struct TownDungeonPersistentData* this)
{
    struct CaveData* caveData = &GetCurrentTownDungeonData()->caveData;
    const struct DummyDungeonCellData* cellData = &caveData->cellsData[GetTownDungeonGamePersistentData()->currentCellIndex];

    // ASSUMING THAT LOCAL WARP ID IS CONNECTIONS INDEX

    const u8 unusedDoorsCapacity = 4;
    u8 unusedDoorsX[unusedDoorsCapacity];
    u8 unusedDoorsY[unusedDoorsCapacity];
    const u8 unusedDoorsCount = GetUnusedWarps(cellData, unusedDoorsX, unusedDoorsY, unusedDoorsCapacity);

    // spawn pickups over unused warps
    for (u8 i = 0; i < unusedDoorsCount; i++)
    {
        //cellData->cellPickupDefinitionCount
        const u8 x = unusedDoorsX[i];
        const u8 y = unusedDoorsY[i];
        [[maybe_unused]] const u8 instanceIndex = SpawnBaseObject(x, y, OBJ_EVENT_GFX_MOVING_BOX);
    }
}

static void SpawnPickups(struct TownDungeonPersistentData* this)
{
    struct CaveData* caveData = &GetCurrentTownDungeonData()->caveData;
    struct DummyDungeonCellData* cellData = &caveData->cellsData[GetTownDungeonGamePersistentData()->currentCellIndex]; // refactor todo get current cell's index from game data
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
    struct TownDungeonPersistentData* currentTownData = GetCurrentTownDungeonData();
    memset(&gameData->objectStaticPickupIndexByInstanceIndex, 1, sizeof(gameData->objectStaticPickupIndexByInstanceIndex));
    SpawnPickups(currentTownData);
    gameData->context = CONTEXT_CAVE;
}