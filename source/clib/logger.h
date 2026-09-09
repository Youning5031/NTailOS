#pragma once

#include "macro.h"
#include "print.h"

#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO  1
#define LOG_LEVEL_WARN  2
#define LOG_LEVEL_ERROR 3
#define LOG_LEVEL_FATAL 4
#define LOG_LEVEL_NONE  5

#if !defined(LOG_STRING)
// clang-format off
#    define LOG_STRING "[%s:%d:%s] %s: "
// clang-format on
#endif

#define _LOG_(level, msg, ...) kprintln(LOG_STRING msg, __FILE_NAME__, __LINE__, __func__, level, ##__VA_ARGS__)

#if !defined(LOG_LEVEL)
#  if defined(DEBUG)
#    define LOG_LEVEL 0
#  else
#    define LOG_LEVEL 1
#  endif
#endif
#if LOG_LEVEL <= LOG_LEVEL_DEBUG
#  define LOG_DEBUG(msg, ...) _LOG_("DEBUG", msg, ##__VA_ARGS__)
#else
#  define LOG_DEBUG(msg, ...) ((void)0)
#endif
#define LOG_debug(msg, ...) LOG_DEBUG(msg, ##__VA_ARGS__)

#if LOG_LEVEL <= LOG_LEVEL_INFO
#  define LOG_INFO(msg, ...) _LOG_("INFO", msg, ##__VA_ARGS__)
#else
#  define LOG_INFO(msg, ...) ((void)0)
#endif
#define LOG_info(msg, ...) LOG_INFO(msg, ##__VA_ARGS__)

#if LOG_LEVEL <= LOG_LEVEL_WARN
#  define LOG_WARN(msg, ...) _LOG_("WARN", msg, ##__VA_ARGS__)
#else
#  define LOG_WARN(msg, ...) ((void)0)
#endif
#define LOG_warn(msg, ...) LOG_WARN(msg, ##__VA_ARGS__)

#if LOG_LEVEL <= LOG_LEVEL_ERROR
#  define LOG_ERROR(msg, ...) _LOG_("ERROR", msg, ##__VA_ARGS__)
#else
#  define LOG_ERROR(msg, ...) ((void)0)
#endif
#define LOG_error(msg, ...) LOG_ERROR(msg, ##__VA_ARGS__)

#if LOG_LEVEL <= LOG_LEVEL_FATAL
#  define LOG_FATAL(msg, ...) _LOG_("FATAL", msg, ##__VA_ARGS__)
#else
#  define LOG_FATAL(msg, ...) ((void)0)
#endif
#define LOG_fatal(msg, ...) LOG_FATAL(msg, ##__VA_ARGS__)

/**
 * @brief 记录日志
 * @param level 日志级别，有 DEBUG/INFO/WARN/ERROR/FATAL 不区分大小写
 * @param msg 日志输出信息
 * @note LOG_STRING 第一个格式化符`%s` `__FILE__`，第二个格式化符`%d` `__LINE__`，第三个格式化符`%s`
 * `__func__`，第四个格式化符`%s`是日志级别，msg中可自定义格式化符
 */
#define LOG(level, msg, ...) LOG_##level(msg, ##__VA_ARGS__)
