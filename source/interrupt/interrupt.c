#include "interrupt.h"

#include "boot/boot.h"
#include "clib/asm/asm.h"
#include "clib/asm/farop.h"
#include "clib/bits.h"
#include "clib/macro.h"
#include "clib/print.h"
#include "clib/string.h"
#include "core/init.h"
#include "core/segment.h"
#include "drivers/apic/apic.h"

#include <stdint.h>

// 定义在`interrupt_asm.asm`中
#define __X(n) extern void isr_##n(void);
LIST_N(255, __X)
#undef __X

INTERRUPT(int_default_handler);

static InterruptDescriptor idt[256] = {
    [0] = IDT_ENTRY(IDT_ATTR_PRESENT | IDT_ATTR_RING0 | IDT_ATTR_INTERRUPT_GATE, 0, 0),
    [1] = IDT_ENTRY(IDT_ATTR_PRESENT | IDT_ATTR_RING0 | IDT_ATTR_TRAP_GATE, 0, 0),
    [2] = IDT_ENTRY(IDT_ATTR_PRESENT | IDT_ATTR_RING0 | IDT_ATTR_INTERRUPT_GATE, 0, 0),
    [3 ... 5] = IDT_ENTRY(IDT_ATTR_PRESENT | IDT_ATTR_RING0 | IDT_ATTR_TRAP_GATE, 0, 0),
    [6 ... 255] = IDT_ENTRY(IDT_ATTR_PRESENT | IDT_ATTR_RING0 | IDT_ATTR_INTERRUPT_GATE, 0, 0),
};

static InterruptHandler int_handler_table[] = {[0 ... 255] = &int_default_handler};

typedef void (*IsrFunc)(void);
BOOT_DATA IsrFunc isr[] = {
#define __X(n) &isr_##n,
    LIST_N(255, __X)
#undef __X
};

static int interrupt_disable_counter = 0;

void interrupt_disable()
{
    if (interrupt_disable_counter == 0) CLI;
    interrupt_disable_counter++;
}

void interrupt_enable()
{
    interrupt_disable_counter--;
    if (interrupt_disable_counter == 0) STI;
}

void register_interrupt_handler(uint64_t vec_n, InterruptHandler handler)
{
    int_handler_table[vec_n] = handler;
}

INTERRUPT(interrupt_route)
{
    int_handler_table[context->vector_number](context);
    return;
}

INTERRUPT(int_default_handler)
{
    kprintln("An illegal interrupt triggered!");
    kprintln("interrupt vector: 0x%lx", context->vector_number);
    kprintln("Address: 0x%lx:0x%lx", context->cs, context->rip);
    HLT;
}

INTERRUPT(general_protection_handler)
{
    kprintln("General Protection Fault");
    kprintln("Address: 0x%lx:0x%lx", context->cs, context->rip);
    HLT;
}

INIT(interrupt)
{
    InterruptDescriptor *pidt = far_arr_ptr(idt);

    for (uint64_t i = 0; i < 256; i++)
    {
        pidt[i].offset_0_15 = BITS((uintptr_t)isr[i], 0, 16);
        pidt[i].offset_16_31 = BITS((uintptr_t)isr[i], 16, 32) >> 16;
        pidt[i].offset_32_63 = BITS((uintptr_t)isr[i], 32, 64) >> 32;
    }
    pidt[8].ist = 1;
    lidt(256 * sizeof(InterruptDescriptor) - 1, pidt);
    far_arr_ptr(int_handler_table)[INT_VEC_GP] = &general_protection_handler;

    if (!apic_init()) return false;

    STI;
    return true;
}
