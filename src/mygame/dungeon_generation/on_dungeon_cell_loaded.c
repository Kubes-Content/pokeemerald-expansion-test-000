#include "global.h"

#include "constants/map_event_ids.h"
#include "event_object_movement.h"
#include "gba/defines.h"
#include "global.fieldmap.h"
#include "constants/event_objects.h"

#include "mygame/persistent/town_dungeon_persistent_data.h"

static void HideInitialInteractableTemplate()
{
    // todo is this the best way to index this? // could we search for the first non-null script? no, what if we needed more scripts?
    struct ObjectEvent* templateObjectEvent = &gObjectEvents[LOCALID_DYNAMIC_INTERACTABLE_TEMPLATE];
    templateObjectEvent->invisible = TRUE;
    MoveObjectEventToMapCoords(templateObjectEvent, 0, 0);
}

// to duplicate/share a poryscript pointer as I don't know of a better way atm
static u8 SpawnLocalCloneFromTemplate(const u8 localId, struct ObjectEventTemplate* template)
{
    template->localId = OBJECT_EVENTS_COUNT; // ensures no collisions

    const u8 count = SpawnSpecialObjectEvent(template);
    if (count != OBJECT_EVENTS_COUNT)
        gObjectEvents[count].localId = localId; // connect script // the template doesn't matter much so long as we have this localId pointing to the intended script

    return count;
}

// to duplicate/share a poryscript pointer as I don't know of a better way atm
static u8 SpawnLocalClone(const u8 localId, s16 x, s16 y, u16 graphicsId)
{
    // test: dynamically spawn another instance ... it'd be nice if the offset was dynamic so I could interact with the next one and see a change
    struct ObjectEventTemplate template = *GetObjectEventTemplateByLocalIdAndMap(localId, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup);
    // TODO is there a way to inject a c function call w/o doing this ^^^
    // it'd give us an extra object per map // but if I can safely bump the max. that doesn't matter
    template.x = x;
    template.y = y;
    template.graphicsId = graphicsId;

    return SpawnLocalCloneFromTemplate(localId, &template);
}

static u8 SpawnDungeonPickup(struct TownDungeonPersistentData* this, u8 staticIndex)
{
    struct DummyPickupDescription* pickupDescription = &this->dummyDungeonData.dummyPickupDescription[staticIndex]; // todo use index
    const u8 instanceIndex = SpawnLocalClone(LOCALID_DYNAMIC_INTERACTABLE_TEMPLATE, pickupDescription->x, pickupDescription->y, pickupDescription->objectEventGraphicsEnum);

    this->dummyDungeonData.staticPickupIndexByInstanceIndex[instanceIndex] = staticIndex; // OnInteract will leverage this

    return instanceIndex;
}

static void SpawnPickups(struct TownDungeonPersistentData* this)
{
    // TODO replace 1 w/ MAX_PICKUPS_PER_DUNGEON
    for (u8 staticIndex = 0; staticIndex < this->dummyDungeonData.caveEntryCellPickupDefinitionCount; staticIndex++)
    {
        if (!this->dummyDungeonData.dummyPickupDescription[staticIndex].isTaken)
            SpawnDungeonPickup(this, staticIndex);
    }
}

// ReSharper disable once CppUseInternalLinkage
void OnDungeonCellLoaded()
{
    struct TownDungeonPersistentData* this = GetCurrentTownDungeonData();
    HideInitialInteractableTemplate();
    SpawnPickups(this);
}