#include "bitmap.h"

#include "bits.h"

#include <stdint.h>

uint64_t bitmap_find_first_free(Bitmap *bitmap)
{
    if (bitmap == nullptr || bitmap->total_bits == 0) return UINT64_MAX;

    uint64_t chunks = bitmap_get_chunk(bitmap->total_bits);

    for (uint64_t i = 0; i < chunks; i++)
    {
        uint64_t data = bitmap->bits[i];
        if (data == UINT64_MAX) continue;
        uint64_t pos = (i << 6) + ctz(~data);
        if (pos < bitmap->total_bits) return pos;
    }
    return UINT64_MAX;
}

uint64_t bitmap_find_first_free_with_start(Bitmap *bitmap, uint64_t start)
{
    if (bitmap == nullptr || bitmap->total_bits == 0 || start >= bitmap->total_bits) return UINT64_MAX;

    uint64_t start_chunk = bitmap_get_chunk(start);
    uint64_t start_offset = bitmap_get_offset(start);
    uint64_t start_data = bitmap->bits[start_chunk] | MASK(start_offset);
    if (start_data != UINT64_MAX)
    {
        uint64_t pos = (start_chunk << 6) + ctz(~start_data);
        if (pos < bitmap->total_bits) return pos;
    }

    for (uint64_t i = start_chunk + 1; i < bitmap_get_chunk(bitmap->total_bits); i++)
    {
        uint64_t data = bitmap->bits[i];
        if (data == UINT64_MAX) continue;
        uint64_t pos = (i << 6) + ctz(~data);
        if (pos < bitmap->total_bits) return pos;
    }
    return UINT64_MAX;
}

uint64_t bitmap_find_contiguous_free_with_start(Bitmap *bitmap, uint64_t start, uint64_t n)
{
    if (bitmap == nullptr || bitmap->total_bits == 0 || start >= bitmap->total_bits || n == 0 || n > bitmap->total_bits)
        return UINT64_MAX;

    uint64_t pos = start;
    do {
        pos = bitmap_find_first_free_with_start(bitmap, pos);
        if (!bitmap_test(bitmap, pos, pos + n)) return pos;
        pos += n;
    } while (pos >= bitmap->total_bits);

    return UINT64_MAX;
}

uint64_t bitmap_count_contiguous_free_size(Bitmap *bitmap, uint64_t start)
{
    if (bitmap == nullptr || bitmap_bit_test(bitmap, start)) return 0;

    uint64_t start_chunk = bitmap_get_chunk(start);
    uint64_t start_offset = bitmap_get_offset(start);
    uint64_t end_chunk = bitmap_get_chunk(bitmap->total_bits);
    uint64_t end_offset = bitmap_get_offset(bitmap->total_bits);

    if (start_chunk == end_chunk)
    {
        uint64_t d = (bitmap->bits[start_chunk] | (MASK(64 - end_offset) << end_offset)) & ~MASK(start_offset);
        int      l = ctz(d) - start_offset;
        return l > 0 ? (uint64_t)l : 0;
    }
    uint64_t len = 0;

    uint64_t sdata = bitmap->bits[start_chunk] & ~MASK(start_offset);
    if (sdata != 0)
    {
        return ctz(sdata) - start_offset;
    }
    len = 64 - start_offset;

    uint64_t cur_chunk = start_chunk + 1;
    while (bitmap->bits[cur_chunk] == 0 && cur_chunk < end_chunk)
    {
        len += 64;
        cur_chunk++;
    }

    uint64_t edata = bitmap->bits[cur_chunk];
    if (cur_chunk == end_chunk) edata |= (MASK(64 - end_offset) << end_offset);
    return len + ctz(edata);
}
