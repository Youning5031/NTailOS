[bits 64]

section .text
    global _kstart
    extern kernel_main
    extern early_stack_top
    extern _bss
    extern _bss_end

_kstart:
    push rcx
    push rax
    push rdi

    mov rcx, _bss_end
    mov rdi, _bss
    sub rcx, rdi
    shr rcx, 3
    rep stosq

    pop rdi
    pop rax
    pop rcx

    mov rsp, early_stack_top
    call kernel_main

.hang:
    hlt
    jmp .hang
