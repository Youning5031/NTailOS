#include "serial.h"

#include "clib/asm/io.h"
#include "core/init.h"

static uint16_t COM1 = 0x3F8;

INIT(serial)
{
    // 参考 16550 UART 的初始化序列
    out(8, COM1 + 1, 0x00);  // 禁用所有中断
    out(8, COM1 + 3, 0x80);  // 设置 DLAB 位（除数锁存访问位），以便设置波特率
    out(8, COM1 + 0, 0x01);  // 设置波特率除数的低字节 (0x01, 结合 0x00 为 0x0100)
    out(8, COM1 + 1, 0x00);  // 设置波特率除数的高字节，最终波特率为 115200
    out(8, COM1 + 3, 0x03);  // 清除 DLAB，并设置为 8位数据，无奇偶校验，1位停止 (8N1)
    out(8, COM1 + 2, 0xC7);  // 启用 FIFO，并清空
    out(8, COM1 + 4, 0x0B);  // 设置数据终端就绪 (DTR) 和请求发送 (RTS)
    return true;
}

/**
 * 检查串口发送保持寄存器是否为空，即是否可以发送下一个字符
 * @return true 表示可以发送，false 表示正忙
 */
bool serial_is_transmit_empty()
{
    // 读取线路状态寄存器 (LSR) 的第 5 位 (0x20)，该位为 1 表示发送保持寄存器为空
    return in(8, COM1 + 5) & 0x20;
}

/**
 * 通过串口发送一个字符
 * @param a 要发送的字符
 */
void serial_write_char(char a)
{
    // 等待，直到串口准备好接收新数据
    while (!serial_is_transmit_empty());
    // 将字符写入数据寄存器
    out(8, COM1, a);
}

/**
 * 通过串口发送一个以`'\0'`结尾的字符串
 * @param str 要发送的字符串
 * @return 发送字符串的个数
 */
int serial_write_string(const char* str)
{
    auto _str = str;
    while (*str)
    {
        serial_write_char(*str);
        str++;
    }
    return str - _str;
}