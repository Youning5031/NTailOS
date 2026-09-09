#pragma once

#include "clib/math.h"
#include "cpu/registers.h"

struct PACKED s_interrupt_descriptor_64
{
    uint16_t offset_0_15;      // offset bits 0-15
    uint16_t selector;         // a code segment selector in GDT or LDT
    uint8_t  ist;              // bits 0-2 holds Interrupt Stack Table offset, rest of bits zero.
    uint8_t  type_attributes;  // gate type, dpl, and p fields
    uint16_t offset_16_31;     // offset bits 16-31
    uint32_t offset_32_63;     // offset bits 32-63
    uint32_t reserved;         // reserved
};

typedef struct s_interrupt_descriptor_64 InterruptDescriptor;

#define IDT_ATTR_PRESENT        (1 << 7)
#define IDT_ATTR_RING0          (0 << 5)  // 内核特权级
#define IDT_ATTR_RING3          (3 << 5)  // 用户特权级
#define IDT_ATTR_INTERRUPT_GATE 0xE
#define IDT_ATTR_TRAP_GATE      0xF

#define IDT_ENTRY(_attr, _addr, _ist)                  \
    {                                                  \
      .type_attributes = (_attr),                      \
      .offset_0_15 = _BITS((uintptr_t)_addr, 0, 16),   \
      .offset_16_31 = _BITS((uintptr_t)_addr, 16, 32), \
      .offset_32_63 = _BITS((uintptr_t)_addr, 32, 64), \
      .ist = _ist,                                     \
      .selector = KERNEL_CS,                           \
      .reserved = 0,                                   \
    }

typedef struct s_interrupt_context
{
    Registers registers;
    uint64_t  vector_number;
    uint64_t  error_code;
    uint64_t  rip;
    uint64_t  cs;
    uint64_t  rflags;
    uint64_t  rsp;
    uint64_t  ss;
} InterruptContext;

#define INTERRUPT(int_name) void int_name(InterruptContext *context)
typedef INTERRUPT((*InterruptHandler));

void interrupt_disable();
void interrupt_enable();
void register_interrupt_handler(uint64_t vec_n, InterruptHandler handler);

INLINE void lidt(uint16_t idt_limit, InterruptDescriptor *idt_addr)
{
    asm("lidt %0" ::"m"((struct PACKED {
        uint16_t             limit;
        InterruptDescriptor *addr;
    }){idt_limit, idt_addr}));
    return;
}

#define INT_VEC_DE  0   // Division Error                 #DE
#define INT_VEC_DB  1   // Debug                          #DB
#define INT_VEC_NMI 2   // Non-maskable Interrupt
#define INT_VEC_BP  3   // Breakpoint                     #BP
#define INT_VEC_OF  4   // Overflow                       #OF
#define INT_VEC_BR  5   // Bound Range Exceeded           #BR
#define INT_VEC_UD  6   // Invalid Opcode                 #UD
#define INT_VEC_NM  7   // Device Not Available           #NM
#define INT_VEC_DF  8   // Double Fault                   #DF
#define INT_VEC_CSO 9   // Coprocessor Segment Overrun
#define INT_VEC_TS  10  // Invalid TSS                    #TS
#define INT_VEC_NP  11  // Segment Not Present            #NP
#define INT_VEC_SS  12  // Stack-Segment Fault            #SS
#define INT_VEC_GP  13  // General Protection Fault       #GP
#define INT_VEC_PF  14  // Page Fault                     #PF
#define INT_VEC_15  15  // Reserved
#define INT_VEC_MF  16  // x87 Floating-Point Exception   #MF
#define INT_VEC_AC  17  // Alignment Check                #AC
#define INT_VEC_MC  18  // Machine Check                  #MC
#define INT_VEC_XM  19  // SIMD Floating-Point Exception  #XM #XF
#define INT_VEC_VE  20  // Virtualization Exception       #VE
#define INT_VEC_CP  21  // Control Protection Exception   #CP
