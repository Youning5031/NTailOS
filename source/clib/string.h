#pragma once

#include "logger.h"

#if __has_builtin(__builtin_memcpy)
#  define _memcpy __builtin_memcpy
#else
#  error "无可用的 memcpy 实现"
#endif

// void *memset(void *s, int c, unsigned long n);
/**
 * @brief `memset`
 * @param s 填充地址
 * @param c 填充内容
 * @param n 填充长度
 * @return
 */
static inline void *memset(void *s, int c, unsigned long n)
{
    unsigned char *p = (unsigned char *)s;
    while (n--)
    {
        *p = (unsigned char)c;
        p++;
    }
    return s;
}

#if __has_builtin(__builtin_strlen)
#  define _strlen __builtin_strlen
#else
#  error "无可用的 strlen 实现"
#endif

/**
 * @brief `strlen`的实现
 * ```
 * unsigned long *strlen(const char *str)
 * ```
 */
#define strlen _strlen