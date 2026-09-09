#pragma once

// clang-format off

#ifdef ASM_FILE
#  define DEFINE(type, name) global name:type (name.end - name)
#else
#  error 只有汇编文件才能使用此头文件！
#endif

// clang-format on
