#define ASM_FILE
#define NEED_MEM_UINTS
#include "mem/phymem.h"
#include "clib/unit.h"
#include "multiboot2.h"
#include "clib/asm/preprocess.h"

section .multiboot2 noexec
    [bits 32]
    DEFINE(data, multiboot2_header)
align 8
multiboot2_header:
    dd MULTIBOOT2_HEADER_MAGIC
    dd MULTIBOOT_ARCHITECTURE_I386
    dd multiboot2_header.end - multiboot2_header
    dd -(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT_ARCHITECTURE_I386 + (multiboot2_header.end - multiboot2_header))

; 请求引导信息 Multiboot2 information request
align 8
information_request_tag:
    dw MULTIBOOT_HEADER_TAG_INFORMATION_REQUEST
    dw 0
    dd information_request_tag.end - information_request_tag
    dd MULTIBOOT_TAG_TYPE_CMDLINE
    dd MULTIBOOT_TAG_TYPE_BASIC_MEMINFO
    dd MULTIBOOT_TAG_TYPE_MMAP
    dd MULTIBOOT_TAG_TYPE_FRAMEBUFFER
    dd MULTIBOOT_TAG_TYPE_MODULE
    dd MULTIBOOT_TAG_TYPE_ELF_SECTIONS
    dd MULTIBOOT_TAG_TYPE_EFI64
    dd MULTIBOOT_TAG_TYPE_EFI_MMAP
.end:

; 入口地址标签
align 8
entry_address_tag:
    dw MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS
    dw 0
    dd entry_address_tag.end - entry_address_tag
    dd _boot_start  ; 入口点物理地址
.end:

; 帧缓冲区 framebuffer tag
align 8
framebuffer_tag:
    dw MULTIBOOT_HEADER_TAG_FRAMEBUFFER
    dw 0
    dd framebuffer_tag.end - framebuffer_tag
    dd 1024
    dd 768
    dd 32
.end:

; 结束
align 8
end_tag:
    dw MULTIBOOT_HEADER_TAG_END
    dw 0
    dd end_tag.end - end_tag
.end:

multiboot2_header.end:


section .boot32 progbits exec
    [bits 32]
    DEFINE(function, _boot_start)
    DEFINE(function, setup_page)
    DEFINE(data, multiboot2_header)
    extern _kstart

_boot_start:
    cli
    push eax ; 魔数
    push ebx ; 引导信息

    ; 建立页表
    call setup_page

    ; 加载GDT
    lgdt [gdt64_ptr]

    ; 启用PAE
    mov eax, cr4
    or eax, 1 << 5       ; CR4.PAE = 1
    mov cr4, eax

    ; 启用长模式
    mov ecx, 0xC0000080  ; EFER MSR
    rdmsr
    or eax, 1 << 8       ; EFER.LME = 1
    wrmsr

    ; 启用分页
    mov eax, cr0
    or eax, 1 << 31      ; CR0.PG = 1
    mov cr0, eax

    pop esi  ; 引导信息
    pop edi  ; 魔数
    jmp 0x0008:bits64

.hang:
    hlt
    jmp .hang
.end:

setup_page:
    push ebp
    mov ebp, esp

    ; setup_pml4
    mov eax, PTE_ATTR_P | PTE_ATTR_W
    or eax, early_pt.pml3
    mov edi, early_pt.pml4
    mov [edi], eax
    mov [edi + 256 * 8], eax
    mov [edi + 511 * 8], eax

    ; setup_pml3
    mov eax, PTE_ATTR_P | PTE_ATTR_W
    or eax, early_pt.pml2
    mov edi, early_pt.pml3
    mov [edi], eax
    mov eax, PTE_ATTR_P | PTE_ATTR_W
    or eax, early_pt.pml2_2
    mov [edi + 510 * 8], eax

    ; setup_pml2 映射
    mov edi, early_pt.pml2
    mov eax, PTE_ATTR_P | PTE_ATTR_W | PTE_ATTR_PS
    mov ecx, 256
.fill_pml2:
    mov [edi], eax
    add edi, 8
    add eax, 1 << 21
    loop .fill_pml2

    ; setup_pml2_2 内核映像映射
    mov edi, early_pt.pml2_2
    mov eax, PTE_ATTR_P | PTE_ATTR_W | PTE_ATTR_PS
    mov ecx, 255
    add eax, 1 << 21
.fill_pml2_2:
    mov [edi], eax
    add edi, 8
    add eax, 1 << 21
    loop .fill_pml2_2

    mov eax, early_pt.pml4
    mov cr3, eax

    leave
    ret
.end:

[bits 64]
    DEFINE(function, bits64)
    DEFINE(data, early_pt)
    DEFINE(data, gdt64)
    DEFINE(data, gdt64_ptr)
bits64:
    mov rax, _kstart
    jmp rax
.end:

align KB(4)
early_pt:
.pml4:
    times 512 dq 0
align KB(4)
.pml3:
    times 512 dq 0
align KB(4)
.pml2:
    times 512 dq 0
align KB(4)
.pml2_2:
    times 512 dq 0
.end:

align 64
gdt64:
.null_entry:
    dq 0
.kernel_code_entry:
    dq 0x00209a0000000000
.kernel_data_entry:
    dq 0x0000920000000000
.end:

gdt64_size equ gdt64.end - gdt64
gdt64_limit equ gdt64_size - 1

gdt64_ptr:
    dw gdt64_limit
    dd gdt64
    dd 0
.end:
