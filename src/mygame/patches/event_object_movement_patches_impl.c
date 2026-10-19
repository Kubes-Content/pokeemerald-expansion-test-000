//
// Created by kubes on 10/18/26.
//
#include "mygame/patches/event_object_movement/event_object_movement_impl.h"

#include <stdlib.h>
#include "global.h"

#include "event_object_movement.h"
#include "fieldmap.h"
#include "random.h"
#include "constants/event_objects.h"

#include "mygame/monster_crossing/save_data/save_block_1_state.h"
#include "mygame/util/overworld_dummy_spawn.h"

static s16 OffsetEdgeCoordinate(s16 loadingEdge, u8 edgeOffset, const s8 cameraDeltaSign)
{
    return loadingEdge + edgeOffset * (cameraDeltaSign * -1);
}

static struct ObjectEvent* SpawnThing(s16 x, s16 y)
{
    /* const s16 debugInitialX = x;
    const s16 debugInitialY = y; */
    
    x = x - MAP_OFFSET; // offset from border-space to usable-space
    y = y - MAP_OFFSET;
    
    struct OutdoorGroundObjectSlot* groundObjectSlot = GetOutdoorGroundObject(GetSaveBlock1State(), x, y);
    if (x >= DEBUG_MAP_X 
        || y >= DEBUG_MAP_Y 
        || groundObjectSlot->objectState.groundObjectType != GROUND_OBJECT_TYPE_FRUIT) // TODO extract handling all ground object types, check if '_NONE here
    {
        return NULL;
    }
    
    fatal_assertf(GetObjectEventIdByXY(x, y) == OBJECT_EVENTS_COUNT);

    //MgbaPrintf(MGBA_LOG_WARN, "object %d, %d (- 1 = %d)", debugInitialX, debugInitialY, debugInitialY-yOffset);
    
    return SpawnDummyObject(x, y, OBJ_EVENT_GFX_POKE_BALL);
    
    /*CreateVirtualObject(OBJ_EVENT_GFX_POKE_BALL, OBJECT_EVENTS_COUNT + 1, x, y, ELEVATION_DEFAULT, DIR_SOUTH); // didn't noticeably help performance in debug build
    return 0;*/
}

// TODO extract iterating a composite of two rectangles
void OnTrySpawnObjectEventsPatch_FnBegin(s16 cameraDeltaX, s16 cameraDeltaY)
{
    struct Coords16 pos = gSaveBlock1Ptr->pos;
    // bounds based on event_object_movement.c:TrySpawnObjectEvents
    s16 left = pos.x - 2;
    s16 right = pos.x + MAP_OFFSET_W + 2;
    s16 top = pos.y;
    s16 bottom = pos.y + MAP_OFFSET_H + 2;

    //MgbaPrintf(MGBA_LOG_WARN, "left %d right %d, top %d, bottom %d", left, right, top, bottom);

    const s8 cameraDeltaXSign = (cameraDeltaX > 0 ? 1 : -1);
    const s8 cameraDeltaYSign = (cameraDeltaY > 0 ? 1 : -1);

    s16 loadingXEdge;
    s16 loadingYEdge;
    s16 rowXLeft;
    s16 rowXRight;
    s16 columnYTop;
    s16 columnYBottom;

    // so, if delta > 0: check that many spaces in the relative direction
        // if delta == 0: use the same formula, but span the whole field without repeating any spots
    if (cameraDeltaX == 0 && cameraDeltaY == 0) // map loaded
    {
        //MgbaPrintf(MGBA_LOG_WARN, "init pos = %d, %d", gSaveBlock1Ptr->pos.x, gSaveBlock1Ptr->pos.y);
        
        cameraDeltaX = right - left + 1 ; // full view area width
        cameraDeltaY = 0;
        loadingXEdge = left;
        loadingYEdge = top;
        rowXLeft = left;
        rowXRight = right;
        columnYBottom = bottom;
        columnYTop = top;

        //return; // TODO load whole view
    }
    else // movement
    {
        if (cameraDeltaX > 0) // x edge = right;
        {
            loadingXEdge = right;
            rowXLeft = left;
            rowXRight = right - cameraDeltaX;
        }
        else // < 0     // x edge = left;
        {
            loadingXEdge = left;
            rowXLeft = left - cameraDeltaX;
            rowXRight = right;
        }

        if (cameraDeltaY > 0) // y edge = bottom;
        {
            loadingYEdge = bottom;
            columnYTop = top;
            columnYBottom = bottom - cameraDeltaY;
        }
        else // < 0     // y edge = top;
        {
            loadingYEdge = top;
            columnYTop = top - cameraDeltaY;
            columnYBottom = bottom;
        }
    }

    //MgbaPrintf(MGBA_LOG_WARN, "loadingXEdge %d + %d leadingYEdge %d + %d", loadingXEdge, cameraDeltaX, loadingYEdge, cameraDeltaY);

    // rows
    for (s16 x = rowXLeft; x <= rowXRight; x++){
        for (u8 yOffset = 0; yOffset < abs(cameraDeltaY); yOffset++)
        {
            const u8 y = OffsetEdgeCoordinate(loadingYEdge, yOffset, cameraDeltaYSign);
            SpawnThing(x, y);
        }
}    
    // columns
    for (s16 y = columnYTop; y <= columnYBottom; y++)
        for (u8 xOffset = 0; xOffset < abs(cameraDeltaX); xOffset++)
        {
            const u8 x = OffsetEdgeCoordinate(loadingXEdge, xOffset, cameraDeltaXSign);
            SpawnThing(x, y);
        }

    // overlapped section
    for (u8 xOffset = 0; xOffset < abs(cameraDeltaX); xOffset++)
        for (u8 yOffset = 0; yOffset < abs(cameraDeltaY); yOffset++)
        {
            // completely untested :)

            const u8 x = OffsetEdgeCoordinate(loadingXEdge, xOffset, cameraDeltaXSign);
            const u8 y = OffsetEdgeCoordinate(loadingYEdge, yOffset, cameraDeltaYSign);
            SpawnThing(x, y);
        }

    // brush in, opposite the delta (bottom brushes up, left brushes right)
        // down is positive, up is negative
}