#include "global.h"

#include "event_data.h"
#include "event_object_movement.h"
#include "main.h"
#include "random.h"
#include "constants/trainer_types.h"
#include "mygame/util/map_header.h"

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
    struct ObjectEvent* gObjectEvent = &gObjectEvents[objectEventIndex];
    gObjectEvent->localId = OBJECT_EVENTS_COUNT + 1;
    gObjectEvent->trainerType = TRAINER_TYPE_OW_WILD_ENCOUNTER; // prevent despawning when offscreen
#define sOverworldEncounterLevel    trainerRange_berryTreeId // from wild_encounter_ow.c
    gObjectEvent->sOverworldEncounterLevel = 128; // prevent despawning when offscreen // from wild_encounter_ow.c:OWE_NO_DESPAWN_FLAG
#undef sOverworldEncounterLevel

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

static void SpawnPickups(struct TownDungeonGamePersistentData* gameData, const struct TownDungeonPersistentData* this, struct TemporaryCaveState* temporaryCaveState, const struct DungeonCellData* cellData)
{
    for (u8 staticIndex = 0; staticIndex < cellData->cellPickupDefinitionCount; staticIndex++)
    {
        if (!PickupIsTaken(cellData, staticIndex))
        {
            SpawnDungeonPickup(staticIndex, gameData, temporaryCaveState, cellData);
        }
    }

    SpawnGarbageOverUnusedDoors(cellData); // TODO move this to OnDungeonCellLoaded
}

[[nodiscard]]
static u8 SpawnMonsterObject(struct TownDungeonGamePersistentData* gameData,
                             struct TemporaryCaveState* temporaryCaveState,
                             struct TownDungeonPersistentData* currentTownData,
                             struct DungeonCellData* cellData,
                             const u8 staticIndex)
{
    const struct CellOverworldMonsterConfig* cellOverworldMonsterConfig = &cellData->monsterConfigs[staticIndex];

    if (cellOverworldMonsterConfig->isDead) return OBJECT_EVENTS_COUNT;

    const struct OverworldMonsterDescription* sharedMonsterDescription = &gameData->sharedMonsterDescriptions[cellOverworldMonsterConfig->sharedDescriptionIndex];
    const u16 graphicsId = sharedMonsterDescription->monSpecies + OBJ_EVENT_MON; // see OBJ_EVENT_GFX_SPECIES
    const u8 instanceIndex = SpawnBaseObject(cellOverworldMonsterConfig->homeX, cellOverworldMonsterConfig->homeY, graphicsId);

    struct ObjectIdentifier* objectIdentifier = &temporaryCaveState->objectIdByInstanceIndex[instanceIndex];
    *objectIdentifier = ObjectIdentifier_Create(OBJ_ID_MONSTER, staticIndex);

    temporaryCaveState->objectInstanceTargetDirections[instanceIndex] = Random() % (CARDINAL_DIRECTION_COUNT - 1) + 1;

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

// TODO check for blocking warps, we don't want them reloading there
static void GetValidDestinationsInDirection(struct ObjectEvent* objectEvent, const enum Direction direction, enum Direction validDirectionsArrOut[2], u8* validDirectionsCountOut)
{
    s16 xOffset, yOffset;
#define MULTIDIR1 xOffset
#define MULTIDIR2 yOffset

    switch (direction)
    {
    case DIR_SOUTH:
    case DIR_NORTH:
    case DIR_WEST:
    case DIR_EAST:
        xOffset = 0;
        yOffset = 0;
        MoveCoords(direction, &xOffset, &yOffset);
        break;
    case DIR_SOUTHWEST:
        MULTIDIR1 = DIR_SOUTH;
        MULTIDIR2 = DIR_WEST;
        goto multidirectional;
    case DIR_SOUTHEAST:
        MULTIDIR1 = DIR_SOUTH;
        MULTIDIR2 = DIR_EAST;
        goto multidirectional;
    case DIR_NORTHWEST:
        MULTIDIR1 = DIR_NORTH;
        MULTIDIR2 = DIR_WEST;
        goto multidirectional;
    case DIR_NORTHEAST:
        MULTIDIR1 = DIR_NORTH;
        MULTIDIR2 = DIR_EAST;
        goto multidirectional;
    default:
        fatalf("invalid direction");
    }

    const enum Collision collisionAtDestination = GetCollisionInDirection(objectEvent, direction);
    const bool8 destinationWillNotCauseCollision = collisionAtDestination == COLLISION_NONE;

    // TODO ObjectAtTileCouldBlockWarp

    if (destinationWillNotCauseCollision)
    {
        validDirectionsArrOut[0] = direction;
        *validDirectionsCountOut = 1;
    }
    else
    {
        *validDirectionsCountOut = 0;
    }

    return;

multidirectional:;

    *validDirectionsCountOut = 0;
    u8 validDirectionsCountTemp = 0;

    GetValidDestinationsInDirection(objectEvent, MULTIDIR1, validDirectionsArrOut, validDirectionsCountOut);
    validDirectionsCountTemp += *validDirectionsCountOut;
    GetValidDestinationsInDirection(objectEvent, MULTIDIR2, *validDirectionsCountOut ? &validDirectionsArrOut[1] : validDirectionsArrOut, validDirectionsCountOut);
    validDirectionsCountTemp += *validDirectionsCountOut;

    *validDirectionsCountOut = validDirectionsCountTemp;

#undef MULTIDIR1
#undef MULTIDIR2
}

static void ForceMoveMonsterInCurrentDir(struct ObjectEvent* objectEvent, const struct TemporaryCaveState* temporaryCaveState)
{
    const u8 objectInstanceId = objectEvent - gObjectEvents;
    enum Direction direction = temporaryCaveState->objectInstanceTargetDirections[objectInstanceId];

    // pick from diagonal options
    if (direction >= CARDINAL_DIRECTION_COUNT)
    {
        enum Direction validDirectionsArr[2];
        u8 validDirectionsCount;
        GetValidDestinationsInDirection(objectEvent, direction, validDirectionsArr, &validDirectionsCount);

        const u8 chosenIndex = validDirectionsCount == 1 ? 0 : Random() % validDirectionsCount;
        direction = validDirectionsArr[chosenIndex];
    }

    ObjectEventSetHeldMovement(objectEvent, GetWalkNormalMovementAction(direction));
}

static u8 DirToBitmask(const enum Direction direction)
{
    const u8 skipDirNoneOffset = 1;

    return 1u << (direction - skipDirNoneOffset);
}

static void TryToMoveNPCInNewDir(struct ObjectEvent* objectEvent, struct TemporaryCaveState* temporaryCaveState, const struct Coords16 currentCoords, const u8 elevation)
{
    enum Direction tempValidDirectionsArr[2];
    u8 tempValidDirectionsCount;
    u8 validDirectionsBitmask = 0; // you couldn't just use a u16 to avoid needing to offset the bit index?

    // check if you can move in any direction

    const u8 skipDirNoneOffset = 1;
    for (u8 dirIndex = skipDirNoneOffset; dirIndex < (u8) CARDINAL_DIRECTION_COUNT; dirIndex++)
    {
        const enum Direction direction = dirIndex;

        GetValidDestinationsInDirection(objectEvent, direction, tempValidDirectionsArr, &tempValidDirectionsCount);

        if (tempValidDirectionsCount) validDirectionsBitmask |= DirToBitmask(tempValidDirectionsArr[0]);
    }

    if (__builtin_popcount(validDirectionsBitmask) == 0) return; // can't move, don't try

    const u8 objectInstanceId = objectEvent - gObjectEvents;
    enum Direction* objectDirectionInCaveDataPtr = &temporaryCaveState->objectInstanceTargetDirections[objectInstanceId];
    const enum Direction initialDirection = *objectDirectionInCaveDataPtr;
    const u8 initialDirectionBitmask = DirToBitmask(initialDirection); // todo get from temp cave data

    if (validDirectionsBitmask == initialDirectionBitmask) return; // don't move in the same direction twice, don't try

    // pick a new direction

        // accumulate relevant directions

    if (validDirectionsBitmask & DirToBitmask(DIR_SOUTH))
    {
        if (validDirectionsBitmask & DirToBitmask(DIR_WEST)) validDirectionsBitmask |= DirToBitmask(DIR_SOUTHWEST);
        if (validDirectionsBitmask & DirToBitmask(DIR_EAST)) validDirectionsBitmask |= DirToBitmask(DIR_SOUTHEAST);
    }
    if (validDirectionsBitmask & DirToBitmask(DIR_NORTH))
    {
        if (validDirectionsBitmask & DirToBitmask(DIR_WEST)) validDirectionsBitmask |= DirToBitmask(DIR_NORTHWEST);
        if (validDirectionsBitmask & DirToBitmask(DIR_EAST)) validDirectionsBitmask |= DirToBitmask(DIR_NORTHEAST);
    }

        // remove previous direction from options

    if (validDirectionsBitmask & initialDirectionBitmask) validDirectionsBitmask &= ~initialDirectionBitmask;

    // select direction

    const u8 validCount = __builtin_popcount(validDirectionsBitmask);
    const u8 chosenBitIndex = Random() % validCount;
    u8 validBitPassed = 0;
    enum Direction chosenDirection = DIR_NONE;

    const u8 bitmaskLength = 8;
    for (u8 i = 0; i < bitmaskLength; i++)
    {
        if (validDirectionsBitmask & DirToBitmask(i + skipDirNoneOffset)) validBitPassed++;

        if (validBitPassed - 1 == chosenBitIndex)
        {
            chosenDirection = i + skipDirNoneOffset;
            break;
        }
    }

    fatal_assertf(chosenDirection != DIR_NONE);

    // set direction in temp data before returning
    *objectDirectionInCaveDataPtr = chosenDirection;

    // move

    ForceMoveMonsterInCurrentDir(objectEvent, temporaryCaveState);
}

// TODO call movement behavior through description
// TODO move relative to a home coordinate with a max distance retrieved via getter
static void TickCaveMonster(struct TemporaryCaveState* temporaryCaveState,
                            const struct CellOverworldMonsterConfig* cellConfig, const u8 objectInstanceIndex)
{
    fatal_assertf(objectInstanceIndex < OBJECT_EVENTS_COUNT);

    if (cellConfig->isDead) return;

    struct ObjectEvent* objectEvent = &gObjectEvents[objectInstanceIndex];

    if (ObjectEventIsHeldMovementActive(objectEvent)
        && !ObjectEventClearHeldMovementIfFinished(objectEvent))
    {
        return;
    }

    // TODO extract below

    // begin calculating next move

    const enum Direction initialTargetDirection = temporaryCaveState->objectInstanceTargetDirections[objectInstanceIndex];
    const struct Coords16 currentCoords = objectEvent->currentCoords;
    const u8 elevation = objectEvent->currentElevation;

    enum Direction validDirectionsArr[2];
    u8 validDirectionsCount;
    GetValidDestinationsInDirection(objectEvent, initialTargetDirection, validDirectionsArr, &validDirectionsCount);

    const u8 cannotMoveInTheCurrentDirection = validDirectionsCount == 0;
    if (cannotMoveInTheCurrentDirection)
    {
        return TryToMoveNPCInNewDir(objectEvent, temporaryCaveState, currentCoords, elevation);
    }
    else
    {
        return ForceMoveMonsterInCurrentDir(objectEvent, temporaryCaveState);
    }
}

void CaveTick(u8 taskId)
{
    if (gMain.inBattle) return;

    // u8* data = (void*) gTasks[taskId].data;

    // if (!gPaletteFade.active) {} // TODO prevent aggro while fading; don't prevent movement

    struct TownDungeonGamePersistentData* gameData = GetTownDungeonGamePersistentData();
    struct TemporaryCaveState* temporaryCaveState = GetTemporaryCaveStatePtr(gameData);
    struct TownDungeonPersistentData* currentTownData = GetCurrentTownDungeonData();
    struct CaveData* caveData = &currentTownData->caveData;
    struct DungeonCellData* cellData = &caveData->cellsData[temporaryCaveState->currentCellIndex];

    u8 queuedBattleMonsterInstanceIndex;
    if (TryGetQueuedBattle_Cave(temporaryCaveState, &queuedBattleMonsterInstanceIndex))
    {
        if (gSpecialVar_Result == 0) return; // ticks until battle start

        // see B_OUTCOME_WON
        MgbaPrintf(MGBA_LOG_WARN, "instance index: %d  gSpecialVar_Result: %d", queuedBattleMonsterInstanceIndex, gSpecialVar_Result);

        struct ObjectIdentifier* objectIdentifier = &temporaryCaveState->objectIdByInstanceIndex[queuedBattleMonsterInstanceIndex];
        fatal_assertf(objectIdentifier->type == OBJ_ID_MONSTER);

        const u8 staticMonsterIndex = objectIdentifier->staticIndex;

        if (gSpecialVar_Result == B_OUTCOME_WON)
        {
            cellData->monsterConfigs[staticMonsterIndex].isDead = TRUE;
            RemoveObjectEvent(&gObjectEvents[queuedBattleMonsterInstanceIndex]);
        }
        // TODO
        if (gSpecialVar_Result == B_OUTCOME_LOST) {}
        if (gSpecialVar_Result == B_OUTCOME_DREW) {}
        if (gSpecialVar_Result == B_OUTCOME_RAN)  {}

        TryClearQueuedBattle_Cave(temporaryCaveState);

        return;
    }

    if (temporaryCaveState->aiWaitDuration > 0)
    {
        temporaryCaveState->aiWaitDuration--;
        return;
    }

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
            TickCaveMonster(temporaryCaveState, cellOverworldMonsterConfig, objectInstanceIndex);
            break;
        default:
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

    gameData->context = CONTEXT_CAVE;
    ClearTemporaryCaveState(temporaryCaveState);
    SpawnPickups(gameData, currentTownData, temporaryCaveState, cellData);
    SpawnMonsters(gameData, temporaryCaveState, currentTownData, cellData);
}