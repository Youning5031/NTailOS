#pragma once

#define GET_ARG_1(arg1, ...)                               arg1
#define GET_ARG_2(arg1, arg2, ...)                         arg2
#define GET_ARG_3(arg1, arg2, arg3, ...)                   arg3
#define GET_ARG_4(arg1, arg2, arg3, arg4, ...)             arg4
#define GET_ARG_5(arg1, arg2, arg3, arg4, arg5, ...)       arg5
#define GET_ARG_6(arg1, arg2, arg3, arg4, arg5, arg6, ...) arg6

#define GET_ARG_AFTER_1(arg1, ...)                               __VA_ARGS__
#define GET_ARG_AFTER_2(arg1, arg2, ...)                         __VA_ARGS__
#define GET_ARG_AFTER_3(arg1, arg2, arg3, ...)                   __VA_ARGS__
#define GET_ARG_AFTER_4(arg1, arg2, arg3, arg4, ...)             __VA_ARGS__
#define GET_ARG_AFTER_5(arg1, arg2, arg3, arg4, arg5, ...)       __VA_ARGS__
#define GET_ARG_AFTER_6(arg1, arg2, arg3, arg4, arg5, arg6, ...) __VA_ARGS__

#define GET_ARG_BEFORE_2(arg1, arg2, ...)                         arg1, arg2
#define GET_ARG_BEFORE_3(arg1, arg2, arg3, ...)                   arg1, arg2, arg3
#define GET_ARG_BEFORE_4(arg1, arg2, arg3, arg4, ...)             arg1, arg2, arg3, arg4
#define GET_ARG_BEFORE_5(arg1, arg2, arg3, arg4, arg5, ...)       arg1, arg2, arg3, arg4, arg5
#define GET_ARG_BEFORE_6(arg1, arg2, arg3, arg4, arg5, arg6, ...) arg1, arg2, arg3, arg4, arg5, arg6

#define COUNT_ARGS(...) GET_ARG_6(, ##__VA_ARGS__, 4, 3, 2, 1, 0, STR(error!))
