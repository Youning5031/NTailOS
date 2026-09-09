#pragma once

#include "clib/macro.h"
#include "stdint.h"

#define _far_get_ptr(sym, _uniq)                           \
    ({                                                     \
        uintptr_t _uniq;                                   \
        asm("movabs %0, %1\n\t" : "=r"(_uniq) : "i"(sym)); \
        (void *)_uniq;                                     \
    })

#define _far_load_val(var, _uniq)                            \
    ({                                                       \
        typeof(var) _uniq;                                   \
        asm("movabs %0, %c1\n\t" : "=a"(_uniq) : "i"(&var)); \
        _uniq;                                               \
    })

#define _far_store_val(var, val, _uniq)                               \
    ({                                                                \
        typeof(var) _uniq = val;                                      \
        asm("movabs %c0, %1\n\t" ::"i"(&var), "a"(_uniq) : "memory"); \
    })

/**
 * @brief 获取远变量、函数的指针
 * @param var 需要以`type_t var`/`type_t *ptr`/`ret_type_t func(...)`定义
 * @return 指针，类型为`typeof(var) *`
 */
#define far_var_ptr(var) ((typeof(var) *)_far_get_ptr(&var, UNIQ_SYM(CONCAT(var, tmp))))

/**
 * @brief 获取远数组首元素的指针
 * @param arr 需要以`type_t arr[n]`定义
 * @return 数组元素指针，类型为`typeof(arr) *`
 */
#define far_arr_ptr(arr) ((typeof(arr[0]) *)_far_get_ptr(arr, UNIQ_SYM(CONCAT(arr, tmp))))

/**
 * @brief 远调用函数
 * @param func 函数名
 * @param args... 参数列表，无参数可以留空
 * @return 函数`func`的返回值
 */
#define far_call(func, args...) far_var_ptr(func)(args)

/**
 * @brief 获取远变量的值
 * @param var 宽度需要满足8、16、32、64
 * @return 值，类型为`typeof(var)`
 */
#define far_load_val(var) _far_load_val(var, UNIQ_SYM(CONCAT(var, tmp)))

/**
 * @brief 修改远变量的值
 * @param var 宽度需要满足8、16、32、64
 * @param val 存入的数据
 */
#define far_store_val(var, val) _far_store_val(var, val, UNIQ_SYM(CONCAT(var, tmp)))
