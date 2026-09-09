ENTRY(_kstart)
SECTIONS
{
  /* . = 0xFFFFFFFF00100000 */

  .text : {
    *(.multiboot2)
    
    *(.text*)
  }

  .data ALIGN(4K) : {
    *(.data*)
    *(.rodata*)
  }
  _data_end = .;

  .bss ALIGN(4K) : {
    _bss = .;
    *(COMMON)
    *(.bss*)
  }
  _bss_end = .;

}