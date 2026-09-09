#pragma once

#include <stddef.h>
#include <stdint.h>

#define EOF (-1)

char *i64toa(int64_t num, char *buf, size_t buf_size, int base, bool is_signed);

/**
 * @brief 通用输出函数，支持64位
 * @param fmt 格式化字符串，格式化标签：`%specifier`
 * @param 可变参数列表 格式化参数，数量不得比格式化标签少，多出的参数将被丢弃
 * @return 输出的字符数量，若失败则返回`EOF`
 * @note specifier:
 * @note c - 一个字符
 * @note s - 以`'\0'`结束的字符串
 * @note d - 十进制数字
 * @note u - 无符号十进制数字
 * @note x - 十六进制数字
 * @note b - 二进制数字
 */
int kprint(const char *fmt, ...);

/**
 * @brief 通用输出函数，支持64位，附带换行
 * @param fmt 格式化字符串，格式化标签：`%specifier`
 * @param 可变参数列表 格式化参数，数量不得比格式化标签少，多出的参数将被丢弃
 * @return 输出的字符数量，若失败则返回`EOF`
 * @note specifier:
 * @note c - 一个字符
 * @note s - 以`'\0'`结束的字符串
 * @note d - 十进制数字
 * @note u - 无符号十进制数字
 * @note x - 十六进制数字
 * @note b - 二进制数字
 */
#define kprintln(fmt, ...) kprint(fmt "\n", ##__VA_ARGS__)