OUTPUT_FORMAT("elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
ENTRY(_boot_start)


PHDRS
{
  /* 引导头 */
  /* R   = 100 = 4 */
  multiboot PT_LOAD FLAGS(4);

  /* 引导代码段 */
  /* R E = 101 = 5 */
  boot_text PT_LOAD FLAGS(5);

  /* 引导读写数据段 */
  /* RW  = 110 = 6 */
  boot_data PT_LOAD FLAGS(6);

  /* 引导只读数据段 */
  /* R   = 100 = 4 */
  boot_rodata PT_LOAD FLAGS(4);

  /* 代码段 */
  /* R E = 101 = 5 */
  text PT_LOAD FLAGS(5);

  /* 读写数据段 */
  /* RW  = 110 = 6 */
  data PT_LOAD FLAGS(6);

  /* 只读数据段 */
  /* R   = 100 = 4 */
  rodata PT_LOAD FLAGS(4);
}

SECTIONS
{
  . = BOOT_PHYS_BASE;

  _boot = .;

  .multiboot2 : {
    _multiboot2 = .;
    KEEP(*(.multiboot2*))
    _multiboot2_end = .;
  } : multiboot

  .boot.text : {
    _boot_text = .;
    *(.boot32*)
    *(.boot.text*)
    *(.boot.startup*)
    _boot_text_end = .;
  } : boot_text

  .boot.data : {
    _boot_data = .;
    *(.boot.data*)
    _boot_data_end = .;
  } : boot_data

  .ctors ALIGN(8) : {
    _ctors_start = .;
    *(SORT(.ctors*))
    _ctors_end = .;
  } : boot_rodata
  _boot_end = .;

  . = KERNEL_SPACE_BASE;
  
  . = VPAGE_BASE;
  _vpage_start = .;
  . = . + VPAGE_NUMBER * VPAGE_SIZE;
  _vpage_end = .;

  . = KERNEL_VIRT_BASE;
  _kernel_start = .;

  .text : AT(KERNEL_PHYS_BASE) {
    _code = .;
    *(.text*)
    _code_end = .;
  } : text

  .rodata ALIGN(4K) : {
    _rodata = .;
    *(.rodata*)
    _rodata_end = .;

    _patchable_function_entries_start = .;
    *(__patchable_function_entries*)
    _patchable_function_entries_end = .;
  } : rodata

  .data ALIGN(4K) : {
    _data = .;
    *(.data*)
    _data_end = .;
  } : data

  .bss ALIGN(4K) : {
    _bss = .;
    *(COMMON)
    *(.bss*)

    . = ALIGN(4096);
    /* 系统栈 */
    early_stack_bottom = .;
    /* 预留8KB */
    . += 8K;
    early_stack_top = .;

    /* 错误栈 */
    error_stack_bottom = .;
    /* 预留4KB */
    . += 4K;
    error_stack_top = .;

    _bss_end = .;
  } : data



  _kernel_end = .;

}