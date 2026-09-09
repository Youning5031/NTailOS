#pragma once

#include "clib/macro.h"
#include "compiler/attribute.h"

#if __has_c_attribute(likely)
#  define LIKELY      [[likely]]
#  define UNLIKELY    [[unlikely]]
#  define likely(x)   x
#  define unlikely(x) x
#else
#  define LIKELY
#  define UNLIKELY
#  define likely(x)   __builtin_expect(!!(x), 1)
#  define unlikely(x) __builtin_expect(!!(x), 0)
#endif

#define INLINE         [[gnu::__always_inline__]] static inline
#define PACKED         [[gnu::__packed__]]
#define SECTION(s)     [[gnu::__section__(STR(s))]]
#define CONSTRUCTOR(n) SECTION(.boot.startup) __attribute__((__constructor__(n)))
#define WEAK           [[gnu::__weak__]]
#define ALIGNED(n)     [[gnu::__aligned__(n)]]
#define USED           [[gnu::__used__]]
#define FALLTHROUGH    __attribute__((fallthrough))

#define per_cpu     SECTION(.percpudata)
#define per_cpu_ptr __seg_fs

#define patch_function(N, M...) __attribute__((__patchable_function_entry__(N, ##M), noinline))

#define unreachable() __builtin_unreachable()

#define types_compatible(type1, type2) __builtin_types_compatible_p(type1, type2)
#define is_type(val, type)             types_compatible(typeof(val1), type)
#define is_same(val1, val2)            types_compatible(typeof(val1), typeof(val2))
#define must_same(val1, val2) \
    static_assert(is_same(val1, val2), "The types of parameters [ " #val1 " ] and [ " #val2 " ] must be compatible")

#define asm __asm__ volatile
