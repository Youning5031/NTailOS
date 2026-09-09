#pragma once

#if __has_builtin(__builtin_popcount)
#  define _popcount(value) __builtin_popcount(value)
#else
#  error "无可用的 popcount 实现"
#endif
/**
 * @brief 返回参数中1的位的个数
 */
#define popcount(value) _popcount(value)

#if __has_builtin(__builtin_ctzg)
#  define _ctz(value) __builtin_ctzg((value), (int)sizeof(value) * 8)
#else
#  error "无可用的 ctz 实现"
#endif
// 返回参数中尾随零的个数
#define ctz(value) _ctz(value)

#if __has_builtin(__builtin_clzg)
#  define _clz(value) __builtin_clzg((value), (int)sizeof(value) * 8)
#else
#  error "无可用的 clz 实现"
#endif
// 返回参数中前导零的个数
#define clz(value) _clz(value)

#if defined(clz)
#  define _bsr(value) ((sizeof(value) * 8 - 1) - clz(value))
#else
#  error "无可用的 bsr 实现"
#endif
// 返回参数第一个1的位置，如果参数为0，返回-1
#define bsr(value) _bsr(value)

#if __has_builtin(__builtin_bswap16) || __has_builtin(__builtin_bswap32) || __has_builtin(__builtin_bswap64)
#  if __has_builtin(__builtin_bswap16)
#    define _BSWAP_CASE16(value) \
    case 2: __builtin_bswap16(value); break
#  else
#    define _BSWAP_CASE16(value)
#    warning "无2字节反转字节序实现"
#  endif

#  if __has_builtin(__builtin_bswap32)
#    define _BSWAP_CASE32(value) \
    case 4: __builtin_bswap32(value); break
#  else
#    define _BSWAP_CASE32(value)
#    warning "无4字节反转字节序实现"
#  endif

#  if __has_builtin(__builtin_bswap64)
#    define _BSWAP_CASE64(value) \
    case 8: __builtin_bswap64(value); break
#  else
#    define _BSWAP_CASE64(value)
#    warning "无8字节反转字节序实现"
#  endif

#  define _biswap(value)        \
      switch (sizeof(value))    \
      {                         \
          _BSWAP_CASE16(value); \
          _BSWAP_CASE32(value); \
          _BSWAP_CASE64(value); \
      }

#else
#  error "无可用的 bswap 实现"
#endif

// 交换字节序
#define bswap(value) _biswap(value)

#define _MASK(n) ((UINTMAX_C(1) << (n)) - 1)

// 获取低`n`位全1的掩码
#define MASK(n)                                       \
    ({                                                \
        auto _n = n;                                  \
        n != UINTMAX_WIDTH ? _MASK(_n) : UINTMAX_MAX; \
    })

#define _BITS(num, m, n) ((((num) >> (m)) & _MASK(n - m)) << m)

// 提取变量`num`范围`[m, n)`的位，从0开始计数
#define BITS(num, m, n)      \
    ({                       \
        auto _m = m;         \
        auto _n = n;         \
        auto _num = num;     \
        _BITS(_num, _m, _n); \
    })
