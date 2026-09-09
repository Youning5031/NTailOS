#pragma once

#include "multiboot2.h"

#include <stdint.h>

typedef struct multiboot_tag BootTag;

typedef struct s_boot_info
{
    uint32_t total_size;
    uint32_t reserved;
    BootTag  tags[];
} BootInfo;

/**
 * @brief 获取引导信息结构体指针
 * @param boot_tag 由引导程序传递来的结构体指针
 * @param except_info_count 需要多少种信息
 * @param type uint32_t 信息种类
 * @param info_pptr BootTag ** 用于接受信息结构体指针的指针
 * @param ... 重复以上两个结构
 * @return 成功获取到信息结构体的个数
 */
uint32_t get_boot_info(BootTag *boot_tag, uint32_t except_info_count, ...);

void print_boot_info(BootInfo *boot_info);

#define BOOT_DATA SECTION(.boot.data)
#define BOOT_CODE SECTION(.boot.text)