#include "global.h"

#include "constants/map_event_ids.h"
#include "event_object_movement.h"
#include "gba/defines.h"
#include "global.fieldmap.h"
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
    struct DummyDungeonCellData* cellData = &this->dungeonCellsData[GetTownDungeonGamePersistentData()->currentCellIndex]; // refactor todo get current cell's index from game data
    const struct DummyPickupDescription* pickupDescription = &cellData->dummyPickupDescription[staticIndex];
    const u8 instanceIndex = SpawnBaseObject(pickupDescription->x, pickupDescription->y, pickupDescription->objectEventGraphicsEnum);//SpawnLocalClone(LOCALID_DYNAMIC_INTERACTABLE_TEMPLATE, pickupDescription->x, pickupDescription->y, pickupDescription->objectEventGraphicsEnum);

    cellData->staticPickupIndexByInstanceIndex[instanceIndex] = staticIndex; // OnInteract will leverage this

    return instanceIndex;
}

static void SpawnPickups(struct TownDungeonPersistentData* this)
{
    const struct DummyDungeonCellData* cellData = &this->dungeonCellsData[GetTownDungeonGamePersistentData()->currentCellIndex]; // refactor todo get current cell's index from game data
    // TODO replace 1 w/ MAX_PICKUPS_PER_DUNGEON
    for (u8 staticIndex = 0; staticIndex < cellData->cellPickupDefinitionCount; staticIndex++)
    {
        if (!cellData->dummyPickupDescription[staticIndex].isTaken)
            SpawnDungeonPickup(this, staticIndex);
    }
}

// ReSharper disable once CppUseInternalLinkage
void OnDungeonCellLoaded()
{
    struct TownDungeonGamePersistentData* gameData = GetTownDungeonGamePersistentData();
    struct TownDungeonPersistentData* currentTownData = GetCurrentTownDungeonData();
    SpawnPickups(currentTownData);
    gameData->context = CONTEXT_CAVE;
}