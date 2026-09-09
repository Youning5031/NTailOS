#pragma once

#include <stdint.h>

// GDT 描述符结构体 (8字节)
struct PACKED s_global_descriptor_64
{
    uint16_t limit_0_15;       // 段限长15-0
    uint16_t base_0_15;        // 基地址15-0
    uint8_t  base_16_23;       // 基地址23-16
    uint8_t  access;           // 访问权限 (P/DPL/Type等)
    uint8_t  limit_16_19 : 4;  // 段限长19-16
    uint8_t  flags       : 4;  // 标志 (G/D/B/L)
    uint8_t  base_24_32;       // 基地址31-24
};

// TSS描述符结构体 (16字节)
struct PACKED s_tss_descriptor_64
{
    uint16_t limit_0_15;       // TSS长度
    uint16_t base_0_15;        // 基地址15-0
    uint8_t  base_16_23;       // 基地址23-16
    uint8_t  access;           // 访问权限
    uint8_t  limit_16_19 : 4;  // 段限长19-16
    uint8_t  flags       : 4;  // 标志 (G/D/B/L)
    uint8_t  base_24_31;       // 基地址31-24
    uint32_t base_32_63;       // 基地址63-32
    uint32_t reserved;         // 保留
};

typedef struct s_global_descriptor_64 GlobalDescriptor;
typedef struct s_tss_descriptor_64    TssDescriptor;

// 访问字节宏
#define GDT_ACCESS_PRESENT  (1 << 7)  // 段存在位
#define GDT_ACCESS_RING0    (0 << 5)  // 内核特权级
#define GDT_ACCESS_RING3    (3 << 5)  // 用户特权级
#define GDT_ACCESS_DATA_SEG (1 << 4)  // 代码/数据段
#define GDT_ACCESS_TSS_SEG  (0 << 4)  // 系统段
#define GDT_ACCESS_EXEC     (1 << 3)  // 可执行
#define GDT_ACCESS_READ     (1 << 1)  // 代码段可读
#define GDT_ACCESS_WRITE    (1 << 1)  // 数据段可写

#define GDT_ACCESS_KERNEL_CODE \
    (GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_DATA_SEG | GDT_ACCESS_EXEC | GDT_ACCESS_READ)
#define GDT_ACCESS_KERNEL_DATA (GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_DATA_SEG | GDT_ACCESS_WRITE)

#define GDT_ACCESS_USER_CODE \
    (GDT_ACCESS_PRESENT | GDT_ACCESS_RING3 | GDT_ACCESS_DATA_SEG | GDT_ACCESS_EXEC | GDT_ACCESS_READ)
#define GDT_ACCESS_USER_DATA (GDT_ACCESS_PRESENT | GDT_ACCESS_RING3 | GDT_ACCESS_DATA_SEG | GDT_ACCESS_WRITE)

/* 32bits
#define GDT_TSS_TYPE_16BITS_AVAI 0x1
#define GDT_TSS_TYPE_LDT         0x2
#define GDT_TSS_TYPE_16BITS_BUSY 0x3
#define GDT_TSS_TYPE_32BITS_AVAI 0x9
#define GDT_TSS_TYPE_32BITS_BUSY 0xB
*/
#define GDT_TSS_TYPE_LDT  0x2
#define GDT_TSS_TYPE_AVAI 0x9
#define GDT_TSS_TYPE_BUSY 0xB

#define GDT_ACCESS_TSS (GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_TSS_SEG | GDT_TSS_TYPE_AVAI)

// 标志字段宏
#define GDT_FLAG_64BIT   (1 << 1)  // 64位代码段
#define GDT_FLAG_32BIT   (1 << 2)  // 32位保护模式
#define GDT_FLAG_GRAN_4K (1 << 3)  // 页粒度(4KB)

#define GDT_FLAG_CODE GDT_FLAG_64BIT
#define GDT_FLAG_DATA 0

// 常用段选择子
#define KERNEL_CS 0x0008  // 内核代码段
#define KERNEL_DS 0x0010  // 内核数据段
#define USER_CS   0x0018  // 用户代码段
#define USER_DS   0x0020  // 用户数据段
#define TSS_SEG   0x0028  // TSS段

// 初始化GDT描述符的辅助宏
/* 32bit
#define GDT_ENTRY(base, limit, access, flags) (GlobalDescriptor){ \
    .limit_0_15 = (limit) & 0xFFFF,                               \
    .base_0_15 = (base) & 0xFFFF,                                 \
    .base_16_23 = ((base) >> 16) & 0xFF,                          \
    .access = (access),                                           \
    .limit_16_19 = ((limit) >> 16) & 0xF,                         \
    .flags = (flags),                                             \
    .base_24_32 = ((base) >> 24) & 0xFF,                          \
}
*/

// clang-format off

#define GDT_ENTRY(_access, _flags)  \
    (GlobalDescriptor) {            \
        .limit_0_15 = 0,            \
        .base_0_15 = 0,             \
        .base_16_23 = 0,            \
        .access = (_access),        \
        .limit_16_19 = 0,           \
        .flags = (_flags),          \
        .base_24_32 = 0,            \
    }

#define GDT_NULL GDT_ENTRY(0, 0)

#define TSS_ENTRY(_base, _limit, _access, _flags)               \
    (TssDescriptor) {                                           \
        .limit_0_15 = (_limit) & 0xFFFF,                        \
        .base_0_15 = (uint64_t)(_base) & 0xFFFF,                \
        .base_16_23 = ((uint64_t)(_base) >> 16) & 0xFF,         \
        .access = (_access),                                    \
        .limit_16_19 = ((_limit) >> 16) & 0xF,                  \
        .flags = (_flags),                                      \
        .base_24_31 = ((uint64_t)(_base) >> 24) & 0xFF,         \
        .base_32_63 = ((uint64_t)(_base) >> 32) & 0xFFFFFFFF,   \
    }
// clang-format on
struct PACKED s_task_state_segment_64
{
    uint32_t  reserved0;
    uintptr_t rsp0;
    uintptr_t rsp1;
    uintptr_t rsp2;
    uint64_t  reserved1;
    uintptr_t ist1;
    uintptr_t ist2;
    uintptr_t ist3;
    uintptr_t ist4;
    uintptr_t ist5;
    uintptr_t ist6;
    uintptr_t ist7;
    uint64_t  reserved2;
    uint16_t  reserved3;
    uint16_t  iopb;
};

typedef struct s_task_state_segment_64 TaskStateSegment;

INLINE void set_data_segment(uint16_t ds)
{
    asm("mov ds, %0\n\t"
        "mov es, %0\n\t"
        "mov ss, %0\n\t"
        :
        : "r"(ds));
    return;
}

INLINE void ltr(uint16_t ts)
{
    asm("ltr %0" ::"r"(ts));
    return;
}

INLINE void lgdt(uint16_t gdt_limit, GlobalDescriptor *gdt_addr, uint16_t ds_ss, uint16_t cs)
{
    asm goto("lgdt %0\n\t"
             "mov ds, %1\n\t"
             "mov es, %1\n\t"
             "mov ss, %1\n\t"
             "mov fs, %4\n\t"
             "mov gs, %4\n\t"
             "push %q2\n\t"
             "push %q3\n\t"
             "retfq\n"
             :
             : "m"((struct PACKED {
                   uint16_t          gdt_limit;
                   GlobalDescriptor *gdt_addr;
               }){gdt_limit, gdt_addr}),
               "rm"(ds_ss), "rm"((uint64_t)cs), "rm"(&&flush), "rm"(0)
             : "memory"
             : flush);
flush:
    return;
}

INLINE void lss(uint16_t ss, uint64_t stack_addr)
{
    asm("lss rsp, %0"
        :
        : "m"((struct PACKED {
            uint16_t ss;
            uint64_t gdt_addr;
        }){ss, stack_addr})
        : "memory");
    return;
}

uint64_t test_gs_64bit_offset(void);
