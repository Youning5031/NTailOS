#pragma once

#include <stdint.h>
#include <x86gprintrin.h>

#define HLT         \
    do {            \
        asm("hlt"); \
    } while (1)

#define STI asm("sti")

#define CLI asm("cli")

#define INT(n) asm("int %0" ::"i"(n))

#define PAUSE __pause()

/*
#ifdef UNIQ_SYM
#  undef UNIQ_SYM
#endif
#define UNIQ_SYM(base_sym) CONCAT(base_sym, __COUNTER__)

// 获取指针
#define _get_ldsym_ptr(ldsym, _ptr_sym)                     \
    ({                                                      \
        uintptr_t _ptr_sym;                                 \
        asm("movabs %0, %1" : "=r"(_ptr_sym) : "i"(ldsym)); \
        (void *)_ptr_sym;                                   \
    })
// 解引用
#define _get_farptr(ldsym, _ptr_sym)                                     \
    ({                                                                   \
        uintptr_t _ptr_sym;                                              \
        asm("movabs %0, QWORD PTR [%c1]" : "=a"(_ptr_sym) : "i"(ldsym)); \
        (void *)_ptr_sym;                                                \
    })

// `extern type_t ldsym[];`
#define get_ldsym_ptr(ldsym) ((typeof(ldsym[0]) *)_get_ldsym_ptr(ldsym, UNIQ_SYM(CONCAT(ldsym, ldsym_ptr))))

// `type_t var;`
#define get_farvar_ptr(var) ((typeof(var) *)_get_ldsym_ptr(&var, UNIQ_SYM(CONCAT(var, farvar_ptr))))

// `type_t arr[n];`
#define get_fararr_ptr(arr) ((typeof(arr[0]) *)_get_ldsym_ptr(arr, UNIQ_SYM(CONCAT(arr, fararr_ptr))))

// `type_t *ptr;`
#define get_farptr(ptr) ((typeof(ptr))_get_farptr(&ptr, UNIQ_SYM(CONCAT(ptr, farptr))))

// `ret_type_t func(...)`
#define get_farfunc_ptr(func) ((typeof(func) *)_get_ldsym_ptr(func, UNIQ_SYM(CONCAT(func, farfunc_ptr))))

// `ret_type_t func(args...)`
#define call_far(func, args...) get_farfunc_ptr(func)(args)
*/
