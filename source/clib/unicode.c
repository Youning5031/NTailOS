#include "unicode.h"

/**
 * @brief 转换字符串的编码
 * @param desc 转换描述符
 * @param src 源字符串
 * @param src_size 源字符串的字节数
 * @param dest 目的字符串
 * @param dest_size 目的字符串的字节数
 * @return 返回目的字符串下一次写入的位置
 */
void *charset_conversion(CSDescriptor *desc, void *src, size_t src_size, void *dest, size_t dest_size)
{
    uint8_t *usrc = src;
    uint8_t *udest = dest;
    while (true)
    {
        // 检查是否结束
        if ((uint8_t *)src + src_size <= usrc)
        {
            desc->errcode = 0;
            return udest;
        }
        // 解码
        size_t code_point;
        switch (desc->from)
        {
        case ASCII:
            code_point = *usrc;
            usrc++;
            break;
        case UTF_8: code_point = utf8_decode_iter((char8_t **)&usrc); break;
        case UTF_16: code_point = utf16_decode_iter((char16_t **)&usrc); break;
        case UTF_32: code_point = utf32_decode_iter((char32_t **)&usrc); break;
        default: desc->errcode = 1; return udest;
        }
        // 编码
        uint8_t buffer[4];
        size_t  conv_len;
        switch (desc->to)
        {
        case UTF_8: conv_len = (size_t)utf8_encode(code_point, (char8_t *)buffer); break;
        case UTF_16: conv_len = (size_t)utf16_encode(code_point, (char16_t *)buffer); break;
        case UTF_32: conv_len = (size_t)utf32_encode(code_point, (char32_t *)buffer); break;
        default: desc->errcode = 1; return udest;
        }
        // 检查空位
        if ((uint8_t *)dest + dest_size < (uint8_t *)udest + conv_len)
        {
            desc->errcode = 2;
            return udest;
        }
        // 写入
        for (size_t i = 0; i < conv_len; i++)
        {
            udest[i] = buffer[i];
        }
        // 移动目标指针
        udest += conv_len;
        // 检查是否结束
        if (!code_point)
        {
            desc->errcode = 0;
            return udest;
        }
    }
}