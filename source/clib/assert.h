#pragma once

#if defined(DEBUG)
#  include "clib/asm/asm.h"
#  include "macro.h"
#  include "print.h"
#  if !defined(ASSERT_STRING)
// clang-format off
#    define ASSERT_STRING "[%s:%d:%s] Assertion Failed: "
// clang-format on
#  endif

#  define assert_print(str, ...) kprintln(ASSERT_STRING str, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
/**
 * @brief 动态断言，当参数为假时触发
 * @param expr 断言表达式
 * @param ... 可选断言信息，可格式化，为空时默认为`expr`
 * @return void
 * @note ASSERT_STRING 第一个格式化符`%s` `__FILE__`，第二个格式化符`%d` `__LINE__`，第三个格式化符`%s`
 * `__func__`
 */
#  define assert(expr, ...)                                                                    \
      ({                                                                                       \
        if (!(expr))                                                                           \
        {                                                                                      \
            COUNT_ARGS(__VA_ARGS__) > 0 ? assert_print(__VA_ARGS__) : assert_print(STR(expr)); \
            HLT;                                                                               \
        }                                                                                      \
      })

#else  // DEBUG

#  define assert(expr) ((void)0)

#endif  // DEBUG
