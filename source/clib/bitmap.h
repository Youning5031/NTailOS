#pragma once

#include "clib/bits.h"

#include <stdint.h>

typedef struct s_bitmap
{
    uint64_t total_bits;  // 比特总数，单位 个
    uint64_t bits[];      // 1表示占用，0表示空闲
} Bitmap;

typedef enum e_bitmap_operation
{
    Bitmap_OP_CLEAR,
    Bitmap_OP_SET,
    Bitmap_OP_TEST,
} BitmapOperation;

#define bitmap_get_map_size(bits_number) (((bits_number) + 63) >> 6)
#define bitmap_get_chunk(index)          ((index) >> 6)
#define bitmap_get_offset(index)         ((index) & ((1ULL << 6) - 1))

INLINE void bitmap_clear(Bitmap *bitmap, uint64_t start, uint64_t end)
{
    if (bitmap == nullptr || start >= bitmap->total_bits || end > bitmap->total_bits) return;

    uint64_t start_chunk = bitmap_get_chunk(start);
    uint64_t start_offset = bitmap_get_offset(start);

    uint64_t end_chunk = bitmap_get_chunk(end);
    uint64_t end_offset = bitmap_get_offset(end);

    if (start_chunk == end_chunk)
    {
        bitmap->bits[start_chunk] &= ~BITS(UINT64_MAX, start_offset, end_offset);
        return;
    }
    bitmap->bits[start_chunk] &= MASK(start_offset);
    for (uint64_t i = start_chunk + 1; i < end_chunk; i++)
    {
        bitmap->bits[i] = 0;
    }
    bitmap->bits[end_chunk] &= ~MASK(end_offset);

    return;
}

INLINE void bitmap_set(Bitmap *bitmap, uint64_t start, uint64_t end)
{
    if (bitmap == nullptr || start >= bitmap->total_bits || end > bitmap->total_bits) return;

    uint64_t start_chunk = bitmap_get_chunk(start);
    uint64_t start_offset = bitmap_get_offset(start);

    uint64_t end_chunk = bitmap_get_chunk(end);
    uint64_t end_offset = bitmap_get_offset(end);

    if (start_chunk == end_chunk)
    {
        bitmap->bits[start_chunk] |= BITS(UINT64_MAX, start_offset, end_offset);
        return;
    }
    bitmap->bits[start_chunk] |= ~MASK(start_offset);
    for (uint64_t i = start_chunk + 1; i < end_chunk; i++)
    {
        bitmap->bits[i] = UINT64_MAX;
    }
    bitmap->bits[end_chunk] |= MASK(end_offset);
    return;
}

INLINE bool bitmap_test(Bitmap *bitmap, uint64_t start, uint64_t end)
{
    if (bitmap == nullptr || start >= bitmap->total_bits || end > bitmap->total_bits) return false;

    uint64_t start_chunk = bitmap_get_chunk(start);
    uint64_t start_offset = bitmap_get_offset(start);

    uint64_t end_chunk = bitmap_get_chunk(end);
    uint64_t end_offset = bitmap_get_offset(end);

    if (start_chunk == end_chunk && (bitmap->bits[start_chunk] & BITS(UINT64_MAX, start_offset, end_offset)) != 0)
        return true;
    if ((bitmap->bits[start_chunk] & ~MASK(start_offset)) != 0) return true;
    for (uint64_t i = start_chunk + 1; i < end_chunk; i++)
    {
        if (bitmap->bits[i] != 0) return true;
    }
    if ((bitmap->bits[end_chunk] & MASK(end_offset)) != 0) return true;
    return false;
}

INLINE void bitmap_bit_clear(Bitmap *bitmap, uint64_t index)
{
    if (bitmap == nullptr || index >= bitmap->total_bits) return;

    uint64_t chunk = bitmap_get_chunk(index);
    uint64_t offset = bitmap_get_offset(index);
    bitmap->bits[chunk] &= ~(1ULL << offset);

    return;
}

INLINE void bitmap_bit_set(Bitmap *bitmap, uint64_t index)
{
    if (bitmap == nullptr || index >= bitmap->total_bits) return;

    uint64_t chunk = bitmap_get_chunk(index);
    uint64_t offset = bitmap_get_offset(index);
    bitmap->bits[chunk] |= (1ULL << offset);

    return;
}

INLINE bool bitmap_bit_test(Bitmap *bitmap, uint64_t index)
{
    if (bitmap == nullptr || index >= bitmap->total_bits) return false;

    uint64_t chunk = bitmap_get_chunk(index);
    uint64_t offset = bitmap_get_offset(index);

    return (bitmap->bits[chunk] & (1ULL << offset));
}

uint64_t bitmap_find_first_free(Bitmap *bitmap);
uint64_t bitmap_find_first_free_with_start(Bitmap *bitmap, uint64_t start);
uint64_t bitmap_find_contiguous_free_with_start(Bitmap *bitmap, uint64_t start, uint64_t n);

INLINE uint64_t bitmap_find_contiguous_free(Bitmap *bitmap, uint64_t n)
{
    return bitmap_find_contiguous_free_with_start(bitmap, 0, n);
}

uint64_t bitmap_count_contiguous_free_size(Bitmap *bitmap, uint64_t start);