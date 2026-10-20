//
// Created by kubes on 10/19/26.
//
#include "mygame/patches/field_control_avatar/try_start_interaction_script_impl.h"

#include "global.h"

#include "event_object_movement.h"
#include "fieldmap.h"
#include "item.h"
#include "script.h"
#include "task.h"

static void PickupGroundObjectAnimationTick(s16 count, s16 pickupTaskDuration, u8 matrixNum)
{
    const s16 spriteScaleOfOne = 256;
    const s16 minScale = 4096;

    s32 normalizedProgress = (count * 256) / pickupTaskDuration;
    s32 curvedProgress = (normalizedProgress * normalizedProgress) >> 8;
    curvedProgress = (curvedProgress * normalizedProgress) >> 8;

    const s16 scale = spriteScaleOfOne + (((minScale - spriteScaleOfOne) * curvedProgress) >> 8);

    gOamMatrices[matrixNum].a = scale;
    gOamMatrices[matrixNum].b = 0;
    gOamMatrices[matrixNum].c = 0;
    gOamMatrices[matrixNum].d = scale;
}
//
static void PickupGroundObjectAnimationTask(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    s16 *counterPtr = &data[0];
    u8 spriteId = data[1];
    u8 matrixNum = data[2];
    struct Sprite *sprite = &gSprites[spriteId];

    const s16 pickupTaskDuration = 64;
    
    // Release player before animation finishes.
    if (*counterPtr == pickupTaskDuration - (pickupTaskDuration * 4 / 5))
    UnlockPlayerFieldControls();
    
    if (*counterPtr == pickupTaskDuration - 1)
    {
        sprite->oam.affineMode = ST_OAM_AFFINE_OFF;
        DestroySprite(sprite);
        FreeOamMatrix(matrixNum);
        DestroyTask(taskId);
        return;
    }
    
    (*counterPtr)++;
    
    PickupGroundObjectAnimationTick(*counterPtr, pickupTaskDuration, matrixNum);
}

// TODO extract // Create a duplicate virtual object at object's position
static struct Sprite* CreateVirtualClone(struct ObjectEvent* objectEvent)
{
    u8 spriteId = CreateVirtualObject(objectEvent->graphicsId,
                                      objectEvent->spriteId, // ???
                                      objectEvent->currentCoords.x - MAP_OFFSET,
                                      objectEvent->currentCoords.y - MAP_OFFSET,
                                      objectEvent->currentElevation,
                                      objectEvent->facingDirection);
    if (spriteId == MAX_SPRITES)
    {
        fatalf();
        return FALSE;
    }

    struct Sprite *sprite = &gSprites[spriteId];

    StartSpriteAnim(sprite, gSprites[objectEvent->spriteId].animNum);

    u8 matrixNum = AllocOamMatrix();
    if (matrixNum == 0xFF)
    {
        fatalf();
        DestroySprite(sprite);
        return FALSE;
    }

    sprite->oam.affineMode = ST_OAM_AFFINE_NORMAL;
    sprite->oam.matrixNum = matrixNum;

    return sprite;
}
//
static u8 CreateTaskChecked(TaskFunc func, u8 priority)
{
    u8 taskId = CreateTask(func, priority);
    if (taskId == NUM_TASKS)
    {
        fatalf();
        return 0;
    }

    return taskId;
}
//
static void PrepDataForTask(s16 taskData[16], struct Sprite *sprite)
{
    taskData[0] = 0; // counter
    taskData[1] = sprite - gSprites; // spriteId
    taskData[2] = sprite->oam.matrixNum;
}
//
static void TakePickupObject(struct OutdoorGroundObjectSlot *groundObjectSlot)
{
    groundObjectSlot->objectState.groundObjectType = GROUND_OBJECT_TYPE_UNINITIALIZED;
    AddBagItem(ITEM_CHERI_BERRY, 1);
}
//
static bool8 TryInteractWithGround(struct OutdoorGroundObjectSlot *groundObjectSlot, struct ObjectEvent *objectEvent)
{
    if (groundObjectSlot->objectState.groundObjectType != GROUND_OBJECT_TYPE_FRUIT) return FALSE; // TODO create function to handle per object type

    struct Sprite *originalSprite = &gSprites[objectEvent->spriteId];
    struct Sprite *sprite = CreateVirtualClone(objectEvent);

    u8 taskId = CreateTaskChecked(PickupGroundObjectAnimationTask, 0);

    PrepDataForTask(gTasks[taskId].data, sprite);

    TakePickupObject(groundObjectSlot);

    RemoveObjectEvent(objectEvent);

    return TRUE;
}

bool8 OnTryStartInteractionScript_FnBegin(const u8 objectEventId)
{
    // TODO if a spawned pickup, text box that says your pockets are full
        // find a better solution than last time, didn't feel very modular
    struct ObjectEvent* objectEvent = &gObjectEvents[objectEventId];
    
    struct Coords16 objectCoords = objectEvent->currentCoords;
        objectCoords.x -= MAP_OFFSET;
        objectCoords.y -= MAP_OFFSET;
    assertf(max(objectCoords.x, objectCoords.y) <= UINT8_MAX);

    if (objectCoords.x >= DEBUG_MAP_X || objectCoords.y >= DEBUG_MAP_Y) return FALSE;

    struct OutdoorGroundObjectSlot* groundObjectSlot = GetOutdoorGroundObject(GetSaveBlock1State(), objectCoords.x, objectCoords.y);
    return TryInteractWithGround(groundObjectSlot, objectEvent);
}
