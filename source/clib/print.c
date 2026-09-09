#include "print.h"

#include "clib/math.h"
#include "clib/string.h"
#include "drivers/serial/serial.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

char *i64toa(int64_t num, char *buf, size_t buf_size, int base, bool is_signed)
{
    if (base != 2 && base != 10 && base != 16) return nullptr;
    if (buf == nullptr || buf_size < 2) return nullptr;

    uint64_t _num;
    bool     is_negative = false;
    if (base == 10 && is_signed && num < 0)
    {
        if (buf_size < 3) return nullptr;  // 结果+符号+终止符至少为3字节
        is_negative = true;
        _num = (uint64_t)abs(num);
        buf_size -= 2;
    }
    else
    {
        _num = (uint64_t)num;
        buf_size -= 1;
    }

    char  tmp[sizeof(num) * 8 + 1];
    char *p = tmp;
    // 转换数字部分（至少输出一位，处理 _num == 0 的情况）
    do {
        int digit = (int)(_num % base);
        _num /= base;
        *p = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
        p++;
    } while (_num > 0);
    *p = '\0';

    char *_buf = buf;
    if (is_negative)  // 添加负号
    {
        _buf[0] = '-';
        _buf++;
    }

    if (p - tmp > buf_size) p = tmp + buf_size;  // 截断超出的结果
    do {
        p--;
        *_buf = *p;
        _buf++;
    } while (p > tmp);
    *_buf = '\0';

    return buf;
}

int early_kprint(const char *fmt, va_list args)
{
    int  count = 0;
    char buf[65];
    union
    {
        char    c;
        int64_t i64;
    } data;
    bool is_signed;
    bool is_i64;
    int  base;

    for (; *fmt; fmt++)
    {
        if (*fmt == '%')
        {
        parse_fmt:
            fmt++;
            switch (*fmt)
            {
            case 's':
                count += serial_write_string(va_arg(args, const char *));
                continue;
                break;

            case 'c':
                data.c = (char)va_arg(args, int);
                goto out_char;
                break;

            case 'd':
                data.i64 = (uint64_t)(is_i64 ? va_arg(args, int64_t) : va_arg(args, int));
                base = 10;
                is_signed = true;
                goto out_num;
                break;

            case 'u':
                data.i64 = (uint64_t)(is_i64 ? va_arg(args, int64_t) : (uint32_t)va_arg(args, int));
                base = 10;
                is_signed = false;
                goto out_num;
                break;

            case 'x':
                data.i64 = (uint64_t)(is_i64 ? va_arg(args, int64_t) : (uint32_t)va_arg(args, int));
                base = 16;
                is_signed = false;
                goto out_num;
                break;

            case 'b':
                data.i64 = (uint64_t)(is_i64 ? va_arg(args, int64_t) : (uint32_t)va_arg(args, int));
                base = 2;
                is_signed = false;
                goto out_num;
                break;

            case 'l':
                is_i64 = true;
                goto parse_fmt;
                break;

            default: return EOF; break;
            }
        }
        else
        {
            data.c = *fmt;
            goto out_char;
        }

    out_char:
        serial_write_char(data.c);
        count++;
        continue;

    out_num:
        if (!i64toa(data.i64, buf, sizeof(buf), base, is_signed)) return EOF;
        count += serial_write_string(buf);
        continue;
    }

    return count;
}

patch_function(5) int kprint(const char *fmt, ...)
{
    va_list args;
    va_start(args);
    auto ret = early_kprint(fmt, args);  // 未来通过函数指针转发
    va_end(args);
    return ret;
}
