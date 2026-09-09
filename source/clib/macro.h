#pragma once

// 将参数转换为字符串
#define __STR(str) #str
#define STR(str)   __STR(str)

// 拼接字符串，使用下划线连接
#define __CONCAT(s1, s2) s1##_##s2
#define CONCAT(s1, s2)   __CONCAT(s1, s2)

// 拼接字符串，没有分隔符
#define __CONCAT_NO(s1, s2) s1##s2
#define CONCAT_NO(s1, s2)   __CONCAT_NO(s1, s2)

#define UNIQ_SYM(base_sym) CONCAT(base_sym, __COUNTER__)

#include "clib/macro/macro_args.h"
#define GET_ARG(n, args...)            GET_ARG_##n(args)
#define GET_ARG_AFTER(n, args...)      GET_ARG_AFTER_##n(args)
#define GET_ARG_BEFORE(n, args...)     GET_ARG_BEFORE_##n(args)
#define GET_ARG_BETWEEN(a, b, args...) GET_ARG_BEFORE_##b(GET_ARG_AFTER_##a(args))

#define DISPATCHER(macro, args...) CONCAT(macro, COUNT_ARGS(args))(args)

#include "clib/macro/macro_list.h"
/**
 * @brief 展开为调用从 0 到 `n` 的指定宏，每次调用传入一个从 0 到 `n` 的索引和可选参数。
 *
 * @param n       整数值 [0, 255] ，指定要生成的宏调用次数。
 * @param macro   要重复调用的宏名称，该宏必须接受至少一个参数（当前索引）。
 * @param args... 传递给 `macro` 的额外参数。（可选）
 *
 * @warning 参数 `n` 必须是预处理期可确定的整数值，范围为 [0, 255] 。
 *
 * @note 依赖于预定义的 `LIST_0` 到 `LIST_255` 宏（见 utils/macro/macro_list.h）。
 *
 * 示例代码：
 * ```
 * #define EXAMPLE_MACRO(i, arg) int x##i = arg + i;
 * LIST_N(3, EXAMPLE_MACRO, 10) // 展开为：int x0 = 10 + 0; int x1 = 10 + 1; int x2 = 10 + 2;
 * ```
 */
#define LIST_N(n, macro, args...) LIST_##n(macro, ##args)

/**
 * @brief 基于_Generic实现泛型函数分发。
 *
 * @param func      函数的基础名称，实际调用时会添加类型后缀。
 * @param type_list 类型列表宏，需接受参数并展开为_Generic的关联列表。
 * @param args...   可变参数，传递给选定的类型特定函数。第一个参数用于类型判断。
 *
 * 示例代码：
 * ```
 * // 定义类型列表宏
 * #define TYPE_LIST(func, args...) int : func##_int(args), double : func##_double(args)
 * // 定义泛型打印宏
 * #define PRINT(args...) GENERIC(print, TYPE_LIST, args)
 * // 使用
 * PRINT(1);    // 调用print_int(1)
 * PRINT(2.0);  // 调用print_double(2.0)
 * ```
 */
#define GENERIC(func, mapping_list, args...) _Generic((GET_ARG_1(args)), mapping_list(func, ##args))
