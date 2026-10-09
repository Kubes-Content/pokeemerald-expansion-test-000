//
// Created by kubes on 9/27/26.
//
#include "mygame/patches/field_control_avatar/try_start_interaction_script/try_start_interaction_script_cave.h"

#include "global.h"

#include "battle_setup.h"
#include "event_object_movement.h"
#include "item.h"
#include "main.h"
#include "menu.h"
#include "script.h"
#include "sound.h"
#include "string_util.h"
#include "task.h"
#include "wild_encounter.h"
#include "constants/songs.h"

#define A_B_START_SELECT (A_BUTTON | B_BUTTON | START_BUTTON | SELECT_BUTTON)

static u32 WindowTest(const u8* text)
{
    // extract window logic
        // TODO WHY IS IT NOT NOTED WHERE YOU FOUND HOW TO DO THIS? DO YOU HATE ME?

    struct WindowTemplate template;

    const u8 topBottom = 15;
    //const u8 topCenter = 8;
    const u8 top = topBottom - 1;
    SetWindowTemplateFields(&template, 0, 1, top, 28, 3, 15, 8);

    const u32 windowId = AddWindow(&template);

    FillWindowPixelBuffer(windowId, PIXEL_FILL(0));
    PutWindowTilemap(windowId);
    //CopyWindowToVram(windowId, COPYWIN_FULL);

    SetStandardWindowBorderStyle(windowId, FALSE);

    DrawStdFrameWithCustomTileAndPalette(
        windowId,
        FALSE,
        0x214,
        14
    );

    AddTextPrinterParameterized(
        windowId,
        FONT_NORMAL,
        text,
        8, 1,
        TEXT_SKIP_DRAW,
        NULL
    );

    CopyWindowToVram(windowId, COPYWIN_FULL);

    return windowId;
}

static void Task_PickupItemObject(const u8 taskId)
{
    const u8 objectId = gTasks[taskId].data[0];
    s16* const tickCount = &gTasks[taskId].data[1];
    s16* const windowId = &gTasks[taskId].data[2];

    const u8 lastPhaseStartIndex = 96;

    if (*tickCount == 0)
    {
        PlaySE(SE_SELECT);
        struct TownDungeonGamePersistentData* gameData = GetTownDungeonGamePersistentData();
        const struct TemporaryCaveState* temporaryCaveState = GetTemporaryCaveStatePtr(gameData);
        struct CaveData* caveData = &GetCurrentTownDungeonData()->caveData;
        // TODO give item or if player is out of room display a message
        // ASSUMING: that this is an object in a dungeon // todo we need a more universal solution longterm
        struct DungeonCellData* cellData = &caveData->cellsData[temporaryCaveState->currentCellIndex];
        const struct ObjectIdentifier* objectIdentifier = &temporaryCaveState->objectIdByInstanceIndex[objectId];
        const u8 pickupStaticIndex = objectIdentifier->staticIndex;
        fatal_assertf(objectIdentifier->type == OBJ_ID_PICKUP);

        *PickupIsTakenPtr(cellData, pickupStaticIndex) = TRUE;

        const struct DummyPickupDescription* pickupDescription = GetPickupDescription(gameData, cellData, pickupStaticIndex);
        const u16 itemEnum = pickupDescription->itemEnum;
        const u8 itemQuantity = 1; // TODO magic number
        AddBagItem(itemEnum, itemQuantity);
        // TODO persist in state like clones did (remove clone functions)
        // TODO popup item description and image // look at how this is done in Emerald
        u8* const itemText = gStringVar1;
        u8* const itemQuantityText = gStringVar2;
        u8* const windowText = gStringVar4;
        CopyItemName(itemEnum, itemText);
        ConvertUIntToDecimalStringN(itemQuantityText, itemQuantity, STR_CONV_MODE_LEFT_ALIGN, 3);
        const u8 src[] = _("Got {STR_VAR_2}x {STR_VAR_1}");
        StringExpandPlaceholders(windowText, src);
        *windowId = WindowTest(windowText); // todo (s16)u32
    }
    else if (*tickCount < 32)
    {
        // allow player to see popup and not button-mash it accidentally
    }
    else if (*tickCount < lastPhaseStartIndex)
    {
        if (*tickCount == 32)
            PlaySE(SE_SELECT);

        if (JOY_NEW(A_B_START_SELECT)) // skip animation and text
        {
            RemoveObjectEvent(&gObjectEvents[objectId]);
            goto destroy_task;
        }
    }
    else
    {
        if (*tickCount == lastPhaseStartIndex) // not needed but this makes me feel better
            RemoveObjectEvent(&gObjectEvents[objectId]); // "animation" end

        if (JOY_NEW(A_B_START_SELECT)) // text box waits for player input to hide
            goto destroy_task;
    }

    if (*tickCount <= lastPhaseStartIndex) // don't let this wrap
        (*tickCount)++;
    return;

destroy_task:
    UnlockPlayerFieldControls();
    DestroyTask(taskId);
    ClearStdWindowAndFrameToTransparent(*windowId, TRUE);
    RemoveWindow(*windowId);
}

// returns TRUE when intercepted
bool8 TryStartInteractionScript_FnBegin_Cave(const u8 objectEventId)
{
    struct TownDungeonGamePersistentData* gameData = GetTownDungeonGamePersistentData();
    if (gameData->context != CONTEXT_CAVE) return FALSE;

    const struct TemporaryCaveState* temporaryCaveState = GetTemporaryCaveStatePtr(gameData);
    const struct CaveData* caveData = &GetCurrentTownDungeonData()->caveData;

    const struct DungeonCellData* currentCaveCellData = &caveData->cellsData[temporaryCaveState->currentCellIndex];
    const struct ObjectIdentifier* objectIdentifier = &temporaryCaveState->objectIdByInstanceIndex[objectEventId];

    switch(objectIdentifier->type)
    {
    case OBJ_ID_NOTHING:
        return TRUE;
    case OBJ_ID_PICKUP:;
        const u8 staticPickupIndex = objectIdentifier->staticIndex;
        if (staticPickupIndex >= currentCaveCellData->cellPickupDefinitionCount) // ???
            return FALSE;

        // TODO give player thing
        const u8 taskId = CreateTask(Task_PickupItemObject, 0);
        gTasks[taskId].data[0] = objectEventId;
        gTasks[taskId].data[1] = 0;
        return TRUE;
    case OBJ_ID_MONSTER:;
        const u8 staticMonsterIndex = objectIdentifier->staticIndex;
        const struct CellOverworldMonsterConfig* config = &currentCaveCellData->monsterConfigs[staticMonsterIndex];
        const u8 gameDataDescriptionIndex = config->sharedDescriptionIndex;
        const struct OverworldMonsterDescription* sharedDescription = &gameData->sharedMonsterDescriptions[gameDataDescriptionIndex];
        CreateWildMon(sharedDescription->monSpecies, 5); // TODO dynamic level
        BattleSetup_StartWildBattle();
        return TRUE;
    default:
        fatalf();
    }
}