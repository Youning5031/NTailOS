#pragma once

#include <stddef.h>
#include <stdint.h>

#define NEED_MEM_UINTS
#include "clib/unit.h"

// Maximum Linear Addresses Bit 最大线性地址宽度
#define LINEAR_ADDRESS_SIZE 48

#define MAX_PHY_MEM_SIZE  TB(64)
#define PAGE_SIZE         KB(4)
#define PAGE_SHIFT        12
#define BOOT_PHYS_BASE    MB(1)
#define KERNEL_PHYS_BASE  MB(2)
#define KERNEL_SPACE_BASE 0xFFFF800000000000
#define VPAGE_BASE        0xFFFF900000000000
#define VPAGE_NUMBER      MAX_PHY_MEM_SIZE / PAGE_SIZE
#define KERNEL_VIRT_BASE  0xFFFFFFFF80000000

typedef struct multiboot_tag BootTag;

void init_mem(BootTag *boot_tags);

void *alloc_pages(uint16_t n);
void  free_pages(void *addr, uint16_t n);

#define alloc_page()    alloc_pages(1)
#define free_page(addr) free_pages((addr), 1)

void *kmalloc(size_t size);
void  kfree(void *addr);

void *mmap();
void  unmmap();

#define kvatopa(va) ((uintptr_t)(va) - KERNEL_SPACE_BASE)
#define kpatova(pa) ((void *)((uintptr_t)(pa) + KERNEL_SPACE_BASE))

#define PAGE_NUM(size) (((size) + PAGE_SIZE) >> PAGE_SHIFT)
