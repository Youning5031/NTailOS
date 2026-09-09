#pragma once

#if __has_builtin(__builtin_alloca)
#  define _alloca __builtin_alloca
#else
#  error "无可用的 alloca 实现"
#endif

/**
 * @brief `alloca`的实现
 * ```
 * void *alloca(unsigned long size)
 * ```
 */
#define alloca _alloca