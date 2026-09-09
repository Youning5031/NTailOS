#pragma once

#include "bits.h"

#include <stdint.h>

typedef enum e_charset
{
    ASCII,

    UTF_8,

    UTF_16,
    UTF_16LE = UTF_16,
    UTF_16BE,

    UTF_32,
    UTF_32LE = UTF_32,
    UTF_32BE,
} Charset;

typedef __SIZE_TYPE__ size_t;

typedef __CHAR8_TYPE__ char8_t;
typedef __CHAR16_TYPE__ char16_t;
typedef __CHAR32_TYPE__ char32_t;

// 获取utf8字符的长度，传入的参数应为有效的编码首字节值，否则返回0
static inline int utf8_charlen(char8_t u8c)
{
    static const int8_t len_tab[9] = {1, 0, 2, 3, 4, 0, 0, 0, 0};
    return len_tab[clz((char8_t)~u8c)];
}

static inline size_t utf8_strlen(const char8_t *u8str)
{
    int len = 0;
    while (*u8str)
    {
        int charlen = utf8_charlen(*u8str);
        u8str += charlen ? charlen : 1;
        len++;
    }
    return len;
}

typedef struct s_unicode_decode_result
{
    uint32_t code_point;
    int      char_bytes;
} UnicodeDecodeResult;

typedef UnicodeDecodeResult U8DecodeResult;

static inline U8DecodeResult utf8_decode(char8_t *u8str)
{
    if (!u8str[0]) return (U8DecodeResult){0, 1};
    int      char_bytes = utf8_charlen(u8str[0]);
    uint32_t codepoint;
    switch (char_bytes)
    {
    case 1: codepoint = u8str[0]; break;
    case 2: codepoint = ((u8str[0] & 0x1F) << 6) | (u8str[1] & 0x3F); break;
    case 3: codepoint = ((u8str[0] & 0x0F) << 12) | ((u8str[1] & 0x3F) << 6) | (u8str[2] & 0x3F); break;
    case 4:
        codepoint = ((u8str[0] & 0x07) << 18) | ((u8str[1] & 0x3F) << 12) | ((u8str[2] & 0x3F) << 6)
                  | (u8str[3] & 0x3F);
        break;
    default:
        codepoint = 0xFFFD;
        char_bytes = 1;
        break;
    }
    return (U8DecodeResult){codepoint, char_bytes};
}

static inline int utf8_encode(uint32_t code_point, char8_t *u8str)
{
    if (code_point < 0x80U)
    {
        u8str[0] = (char8_t)code_point;
        return 1;
    }
    else if (code_point < 0x800U)
    {
        u8str[0] = (char8_t)(code_point >> 6) | 0b11000000U;
        u8str[1] = (char8_t)(code_point & 0b111111U) | 0b10000000U;
        return 2;
    }
    else if (code_point < 0xD800U || (code_point >= 0xE000U && code_point < 0x10000U))
    {
        u8str[0] = (char8_t)(code_point >> 12) | 0b11100000U;
        u8str[1] = (char8_t)(code_point >> 6 & 0b111111U) | 0b10000000U;
        u8str[2] = (char8_t)(code_point & 0b111111U) | 0b10000000U;
        return 3;
    }
    else if (code_point >= 0x10000U && code_point < 0x110000U)
    {
        u8str[0] = (char8_t)(code_point >> 18) | 0b11110000U;
        u8str[1] = (char8_t)(code_point >> 12 & 0b111111U) | 0b10000000U;
        u8str[2] = (char8_t)(code_point >> 6 & 0b111111U) | 0b10000000U;
        u8str[3] = (char8_t)(code_point & 0b111111U) | 0b10000000U;
        return 4;
    }
    else
    {
        return utf8_encode(0xFFFDU, u8str);
    }
}

static inline uint32_t utf8_decode_iter(char8_t **pu8str)
{
    if (!**pu8str) return 0;
    U8DecodeResult ret = utf8_decode(*pu8str);
    *pu8str += ret.char_bytes;
    return ret.code_point;
}

// 获取utf16字符的长度，传入的参数应为有效的编码首字节值，否则返回0
// 使用小端序，大端序字符传入前需要先转换
static inline int utf16_charlen(char16_t u16c)
{
    if (u16c >= 0xD800U && u16c < 0xDC00U) return 4;
    else if (u16c >= 0xDC00U && u16c < 0xE000U) return 0;
    else return 2;
}

// 使用小端序，大端序字符串传入前需要先转换
static inline size_t utf16_strlen(const char16_t *u16str)
{
    int len = 0;
    while (*u16str)
    {
        int charlen = utf16_charlen(*u16str);
        u16str = (const char16_t *)((uint8_t *)u16str + (charlen ? charlen : 2));
        len++;
    }
    return len;
}

typedef UnicodeDecodeResult U16DecodeResult;

static inline U16DecodeResult utf16_decode(char16_t *u16str)
{
    if (!u16str[0]) return (U16DecodeResult){0, 2};
    int      char_bytes = utf16_charlen(u16str[0]);
    uint32_t codepoint;
    switch (char_bytes)
    {
    case 2: codepoint = u16str[0]; break;
    case 4:
        if (u16str[1] >= 0xDC00U && u16str[1] < 0xE000U)
        {
            codepoint = ((u16str[0] - 0xD800U) << 10) | (u16str[1] - 0xDC00U);
            break;
        }
        FALLTHROUGH;
    default:
        codepoint = 0xFFFDU;
        char_bytes = 2;
        break;
    }
    return (U16DecodeResult){codepoint, char_bytes};
}

static inline int utf16_encode(uint32_t code_point, char16_t *u16str)
{
    if (code_point < 0xD800U || (code_point >= 0xE000U && code_point < 0x10000U))
    {
        u16str[0] = (char16_t)code_point;
        return 2;
    }
    else if (code_point >= 0x10000U && code_point < 0x110000U)
    {
        uint32_t bmp = (code_point - 0x10000U);
        u16str[0] = (char16_t)(bmp >> 10) + 0xD800U;
        u16str[1] = (char16_t)(bmp & MASK(10)) + 0xDC00U;
        return 4;
    }
    else return utf16_encode(0xFFFD, u16str);
}

static inline uint32_t utf16_decode_iter(char16_t **pu16str)
{
    if (!**pu16str) return 0;
    U16DecodeResult ret = utf16_decode(*pu16str);
    *pu16str = (char16_t *)((uint8_t *)*pu16str + ret.char_bytes);
    return ret.code_point;
}

static inline size_t utf32_strlen(const char32_t *u32c)
{
    const char32_t *p = u32c;
    while (*p) p++;
    return p - u32c;
}

typedef UnicodeDecodeResult U32DecodeResult;

static inline U32DecodeResult utf32_decode(char32_t *u32str)
{
    uint32_t code_point;
    if (*u32str < 0xD800U || (*u32str >= 0xE000U && *u32str < 0x10000U)) code_point = *u32str;
    else code_point = 0xFFFDU;
    return (U32DecodeResult){code_point, 4};
}

static inline int utf32_encode(uint32_t code_point, char32_t *u32str)
{
    if (code_point < 0xD800U || (code_point >= 0xE000U && code_point < 0x10000U)) u32str[0] = code_point;
    else u32str[0] = 0xFFFDU;
    return 4;
}

static inline uint32_t utf32_decode_iter(char32_t **pu32str)
{
    if (!**pu32str) return 0;
    U32DecodeResult ret = utf32_decode(*pu32str);
    *pu32str += 1;
    return ret.code_point;
}

typedef struct s_charset_conversion_descriptor
{
    const Charset from;
    const Charset to;
    size_t        errcode;
} CSDescriptor;

#define CRATE_CSDescriptor(from, to) \
    (CSDescriptor)                   \
    {                                \
        from, to                     \
    }
#define INIT_CSDescriptor(name, from, to) CSDescriptor name = CRATE_CSDescriptor(from, to);

void *charset_conversion(CSDescriptor *desc, void *src, size_t src_size, void *dest, size_t dest_size);