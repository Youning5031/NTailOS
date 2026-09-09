#include "segment.h"

#include "clib/asm/farop.h"
#include "init.h"

extern uint8_t error_stack_top[];

static TaskStateSegment tss = {
    .rsp0 = (uintptr_t)error_stack_top,  // 直到第一个用户进程启动再修改
    .ist1 = (uintptr_t)error_stack_top,
    .iopb = sizeof(TaskStateSegment),
};

static GlobalDescriptor gdt_entries[7] = {
    [0] = GDT_NULL,
    [KERNEL_CS >> 3] = GDT_ENTRY(GDT_ACCESS_KERNEL_CODE, GDT_FLAG_CODE),
    [KERNEL_DS >> 3] = GDT_ENTRY(GDT_ACCESS_KERNEL_DATA, GDT_FLAG_DATA),
    [USER_CS >> 3] = GDT_ENTRY(GDT_ACCESS_USER_CODE, GDT_FLAG_CODE),
    [USER_DS >> 3] = GDT_ENTRY(GDT_ACCESS_USER_CODE, GDT_FLAG_DATA),
    [5 ... 6] = GDT_NULL,  // 为TSS项预留
};

INIT(segment)  // init gdt & tss
{
    TssDescriptor *tss_entry = (TssDescriptor *)(far_arr_ptr(gdt_entries) + (TSS_SEG >> 3));
    *tss_entry = TSS_ENTRY(&tss, sizeof(TaskStateSegment) - 1, GDT_ACCESS_TSS, 0);

    lgdt(7 * sizeof(GlobalDescriptor) - 1, far_arr_ptr(gdt_entries), KERNEL_DS, KERNEL_CS);
    return true;
}


//------------------------
#include "clib/asm/msr.h"
// 放在 .data 段中的一个测试变量（假设链接脚本将其放在高半区规范地址，如 0xFFFFFFFF8000xxxx）
static uint64_t __attribute__((used)) secret_data = 0x1122334455667788;

/**
 * 测试使用 64 位偏移量（甚至是非规范偏移量）通过 GS 段访问内存
 * 返回读取到的 64 位值
 */
uint64_t test_gs_64bit_offset(void) {
    // 1. 设定一个目标线性地址（低半区规范地址，例如 0x1000）
    //    在 QEMU 裸机中，低 1MB 通常已被映射，0x1000 可读。
    const uint64_t target_addr = (uint64_t)&secret_data;

    // 2. 设定 GS 段基址为一个高半区规范地址
    //    0xFFFF800000000000 符合规范（位 47 为 1，高 16 位全是 1）
    const uint64_t gs_base = 0xFFFFFFFF80000000ULL;

    // 3. 计算 64 位偏移量：使得 基址 + 偏移量 = target_addr (模 2^64)
    //    计算得到的 0x0000800000001000 是一个非规范地址（位47=1, 高16位=0x0000）
    const uint64_t * huge_offset = (uint64_t *)(target_addr - gs_base);

    writemsr(IA32_GS_BASE,gs_base);
    
    uint64_t result = 0;

    __asm__ volatile (
        "mov %[result], gs:[%[offset]]\n\t"

        : [result] "=a"(result)
        : [offset] "ri"(huge_offset)
        : "memory"
    );

    return result;
}