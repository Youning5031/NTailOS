#pragma once

#include <stdint.h>

#define DATA_REGISTERS     uint64_t rax, rbx, rcx, rdx
#define DATA_REGISTERS_32  uint32_t eax, ebx, ecx, edx
#define EXTENDED_REGISTERS uint64_t r8, r9, r10, r11, r12, r13, r14, r15
#define POINTER_REGISTERS  uint64_t rsp, rbp
#define INDEX_REGISTERS    uint64_t rsi, rdi

typedef struct s_data_registers
{
    DATA_REGISTERS;
} DataRegisters;

typedef struct s_data_registers_32
{
    DATA_REGISTERS_32;
} DataRegisters32;

typedef struct s_index_registers
{
    INDEX_REGISTERS;
} IndexRegisters;

typedef struct s_extended_registers
{
    EXTENDED_REGISTERS;
} ExtendedRegisters;

typedef struct s_registers
{
    DATA_REGISTERS;
    INDEX_REGISTERS;
    EXTENDED_REGISTERS;
} Registers;