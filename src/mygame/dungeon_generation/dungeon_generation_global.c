//
// Created by kubes on 9/28/26.
//
#include "mygame/dungeon_generation/dungeon_generation_global.h"

#include "overworld.h"
#include "random.h"
#include "mygame/util/MapHeader.h"

struct CellVariant CellVariant_CreateEmpty()
{
    return CellVariant_Create(0,0,0,0,0);
}

struct CellVariant CellVariant_Create(u16 mapEnum, bool8 hasNorthWarp, bool8 hasSouthWarp, bool8 hasEastWarp, bool8 hasWestWarp)
{
    return (struct CellVariant) {
        .mapEnum = mapEnum,
        .hasNorthWarp = hasNorthWarp,
        .hasSouthWarp = hasSouthWarp,
        .hasEastWarp = hasEastWarp,
        .hasWestWarp = hasWestWarp,
    };
}

struct RelativeCellConnection RelativeCellConnection_Create(const enum Connection direction)
{
    fatal_assertf(direction > CONNECTION_NONE);
    fatal_assertf(direction < CONNECTION_DIVE); // for now >:)
    return (struct RelativeCellConnection) {
        .direction = direction
    };
}

// todo extract
static void GetAllCellMapEnums(u16* const mapEnumArray, const u8 count)
{
    for (u8 i = 0; i < count; i++)
        mapEnumArray[i] = MAP_CAVE_TOWN_DUNGEON_ROOM_TEST_01;
}

static struct CellVariant DetermineCellVariant(const enum MapEnum mapEnum)
{
    const struct MapHeader* const mapHeader = Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(mapEnum), MAP_NUM(mapEnum));
    const u8 allWarpsArrLength = 10;
    struct WarpEvent allWarpsArr[allWarpsArrLength];
    const u8 warpCount = GetAllWarps(mapEnum, allWarpsArr, allWarpsArrLength);

    struct CellVariant cell = CellVariant_CreateEmpty();
    cell.mapEnum = mapEnum;

    for (u8 warpIndex = 0; warpIndex < warpCount; warpIndex++)
    {
        const s32 width = mapHeader->mapLayout->width;
        const s32 height = mapHeader->mapLayout->height;
        const s16 x = allWarpsArr[warpIndex].x;
        const s16 y = allWarpsArr[warpIndex].y;

        if (x < 2)
            cell.hasWestWarp = TRUE;
        else if (x >= width - 2)
            cell.hasEastWarp = TRUE;
        else if (y < 2)
            cell.hasNorthWarp = TRUE;
        else if (y >= height - 2)
            cell.hasSouthWarp = TRUE;
    }

    fatal_assertf(cell.hasNorthWarp || cell.hasSouthWarp || cell.hasWestWarp || cell.hasEastWarp);

    return cell;
}

static bool8 CellsCanConnect(const struct CellVariant* A, const struct RelativeCellConnection connection, const struct CellVariant* B)
{
    switch (connection.direction)
    {
    case CONNECTION_NORTH:
        return A->hasNorthWarp && B->hasSouthWarp;
    case CONNECTION_SOUTH:
        return A->hasSouthWarp && B->hasNorthWarp;
    case CONNECTION_EAST:
        return A->hasEastWarp && B->hasWestWarp;
    case CONNECTION_WEST:
        return  A->hasWestWarp && B->hasEastWarp;
    case CONNECTION_INVALID:
    case CONNECTION_NONE:
    case CONNECTION_DIVE:
    case CONNECTION_EMERGE:
    default:
        fatal_assertf(FALSE && "invalid connection enum value.");
    }
}

// returns number of elements written into array
[[nodiscard]] static u8 GetAllApplicableCellVariants(const struct CellVariant* const A,
                                                     const struct RelativeCellConnection connectionType,
                                                     struct CellVariant* returnedArray, u8 arraySize)
{
    // TODO actually determine which of the available cell variants are applicable

    fatal_assertf(arraySize == 1);

    const u8 mapEnumArraySize = 1;
    u16 cellMapEnums[mapEnumArraySize];
    GetAllCellMapEnums(cellMapEnums, mapEnumArraySize);
    returnedArray[0] = DetermineCellVariant(cellMapEnums[Random() % mapEnumArraySize]);

    return 1;
}

// TODO
struct CellVariant GetConnectableCell(const struct CellVariant* const A, const struct RelativeCellConnection connectionType)
{
    const u8 arraySize = 1;
    struct CellVariant applicableCellVariants[arraySize];
    const u8 applicableCellVariantsFound = GetAllApplicableCellVariants(A, connectionType, applicableCellVariants, arraySize);
        fatal_assertf(applicableCellVariantsFound == 1);

    const struct CellVariant B = applicableCellVariants[Random() % applicableCellVariantsFound];

    fatal_assertf(CellsCanConnect(A, connectionType, &B));

    return B;
}

u16 GetCellVariantMapEnum(const struct CellVariant* cellVariant)
{
    return cellVariant->mapEnum;
}

bool8 RelativeCellConnection_Equal(const struct RelativeCellConnection* A, const struct RelativeCellConnection* B)
{
    return A->direction == B->direction;
}
