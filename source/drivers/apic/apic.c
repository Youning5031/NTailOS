#include "apic.h"

#include "boot/boot.h"
#include "clib/asm/farop.h"
#include "clib/asm/io.h"
#include "clib/asm/msr.h"
#include "clib/assert.h"
#include "clib/bits.h"
#include "cpu/cpuid.h"

// 不信任bootloader已经初始化PIC，我们自己初始化
BOOT_CODE void pic_init(void)
{
    // 初始化主片
    out(8, PIC_MST_EVE_PORT, 0x11);  // ICW1
    out(8, PIC_MST_ODD_PORT, 0x20);  // ICW2
    out(8, PIC_MST_ODD_PORT, 0x04);  // ICW3
    out(8, PIC_MST_ODD_PORT, 0x01);  // ICW4

    // 初始化从片
    out(8, PIC_SLV_EVE_PORT, 0x11);  // ICW1
    out(8, PIC_SLV_ODD_PORT, 0x28);  // ICW2
    out(8, PIC_SLV_ODD_PORT, 0x02);  // ICW3
    out(8, PIC_SLV_ODD_PORT, 0x01);  // ICW4
}

BOOT_CODE void pic_disable(void)
{
    // 屏蔽
    out(8, PIC_MST_ODD_PORT, 0xFF);
    out(8, PIC_SLV_ODD_PORT, 0xFF);
    // 发送EOI，清除未完成的中断
    out(8, PIC_MST_EVE_PORT, 0x20);
    out(8, PIC_SLV_EVE_PORT, 0x20);
}

BOOT_CODE bool apic_init(void)
{
    CpuidCache *pcpuid_cache = far_var_ptr(cpuid_cache);
    if (!cpuid(pcpuid_cache, 0x1).x2apic) return false;

    pic_init();
    pic_disable();

    uint64_t apic_base_msr = readmsr(IA32_APIC_BASE);
    apic_base_msr = (apic_base_msr & ~(0x1UL << 10 | 0x1UL << 11)) | 0x1UL << 8;
    writemsr(IA32_APIC_BASE, apic_base_msr);
    apic_base_msr |= 0x1UL << 11;
    writemsr(IA32_APIC_BASE, apic_base_msr);
    apic_base_msr |= 0x1UL << 10;
    writemsr(IA32_APIC_BASE, apic_base_msr);

    return true;
}
