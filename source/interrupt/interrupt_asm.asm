extern interrupt_route

section .text
; 定义一个宏，生成一个中断处理程序的入口桩
%macro ISR_NOERRORCODE 1  ; 参数 %1 是中断向量号
global isr_%1:function    ; 声明为全局符号，以便C代码可以设置IDT
isr_%1:
    push qword 0          ; 压入虚假错误码
    push qword %1         ; 压入中断向量号
    jmp interrupt_common_stub ; 跳转到统一的公共处理代码
%endmacro

%macro ISR_ERRORCODE 1   ; 有错误码的中断
global isr_%1:function
isr_%1:
                         ; CPU已经压入了错误码，这里不需要再压入0
    push qword %1        ; 压入中断向量号
    jmp interrupt_common_stub
%endmacro

section .text

ISR_NOERRORCODE 0   ; Division Error                #DE
ISR_NOERRORCODE 1   ; Debug                         #DB
ISR_NOERRORCODE 2   ; Non-maskable Interrupt        #NMI
ISR_NOERRORCODE 3   ; Breakpoint                    #BP
ISR_NOERRORCODE 4   ; Overflow                      #OF
ISR_NOERRORCODE 5   ; Bound Range Exceeded          #BR
ISR_NOERRORCODE 6   ; Invalid Opcode                #UD
ISR_NOERRORCODE 7   ; Device Not Available          #NM
ISR_NOERRORCODE 8   ; Double Fault                  #DF
ISR_NOERRORCODE 9   ; Coprocessor Segment Overrun   #CSO
ISR_ERRORCODE   10  ; Invalid TSS                   #TS
ISR_ERRORCODE   11  ; Segment Not Present           #NP
ISR_ERRORCODE   12  ; Stack-Segment Fault           #SS
ISR_ERRORCODE   13  ; General Protection Fault      #GP
ISR_ERRORCODE   14  ; Page Fault                    #PF
ISR_NOERRORCODE 15  ; Reserved
ISR_NOERRORCODE 16  ; x87 Floating-Point Exception  #MF
ISR_ERRORCODE   17  ; Alignment Check               #AC
ISR_NOERRORCODE 18  ; Machine Check                 #MC
ISR_NOERRORCODE 19  ; SIMD Floating-Point Exception #XM #XF
ISR_NOERRORCODE 20  ; Virtualization Exception      #VE
ISR_ERRORCODE   21  ; Control Protection Exception  #CP

%assign i 22
%rep 256-22          ; 生成剩余的中断桩
ISR_NOERRORCODE i
%assign i i+1
%endrep

; 这是所有中断最终都会跳转到的公共存根
; 保存所有寄存器、调用C路由器、恢复寄存器
interrupt_common_stub:
    push r15
    push r14
    push r13
    push r12
    push r11
    push r10
    push r9
    push r8
    push rdi
    push rsi
    push rdx
    push rcx
    push rbx
    push rax

    mov rdi, rsp  ; 将栈顶指针（指向interrupt_frame）作为第一个参数传递
    push rsp
    and rsp, -16
    call interrupt_route  ; 调用统一的C处理函数
    pop rsp
    
    pop rax
    pop rbx
    pop rcx
    pop rdx
    pop rsi
    pop rdi
    pop r8
    pop r9
    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15

    add rsp, 16      ; 清理向量号和错误码
    iretq            ; 从中断返回
.end:
