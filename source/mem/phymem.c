#include "phymem.h"

#define NEED_MEM_UINTS
#include "clib/asm/asm.h"
#include "clib/bits.h"
#include "clib/list.h"
#include "clib/lock.h"
#include "clib/logger.h"
#include "clib/print.h"
#include "clib/string.h"
#include "clib/unit.h"
#include "memory.h"

#include <stdint.h>

// static MemDescriptor *kernel;

PageTable *get_next_level_page_table(PageTableEntry *pte, uint64_t flags)
{
    if (!pte->present)
    {
        PageTable *new_page = (PageTable *)alloc_page();
        if (!new_page) return nullptr;
        memset(new_page, 0, PAGE_SIZE);
        pte->page_table_entry = kvatopa(new_page) | flags | PTE_ATTR_P;
        return new_page;
    }
    else
    {
        return (PageTable *)kpatova(pte->addr << 12);
    }
}

PageTable *addr_mapping(PageTable *ptbase, uintptr_t paddr, uintptr_t vaddr, uint64_t flags)
{
    // LOG(debug, "page table: 0x%lx, mapping 0x%lx,0x%lx", ptbase, vaddr, paddr);
    if (!ptbase)
    {
        ptbase = alloc_page();
        if (!ptbase) goto PML4_CRATE_FAILED;
        memset(ptbase, 0, PAGE_SIZE);
    }

    flags = flags & ~BITS(UINT64_MAX, 12, 52);

    uint64_t pml4_index = (vaddr >> 39) & 0x1FFU;
    uint64_t pml3_index = (vaddr >> 30) & 0x1FFU;
    uint64_t pml2_index = (vaddr >> 21) & 0x1FFU;
    uint64_t pml1_index = (vaddr >> 12) & 0x1FFU;

    PageTable *pml3 = get_next_level_page_table(&ptbase->pte[pml4_index], flags);
    if (!pml3) goto PML3_CRATE_FAILED;

    PageTable *pml2 = get_next_level_page_table(&pml3->pte[pml3_index], flags);
    if (!pml2) goto PML2_CRATE_FAILED;

    PageTable *pml1 = get_next_level_page_table(&pml2->pte[pml2_index], flags);
    if (!pml1) goto PML1_CRATE_FAILED;

    PageTableEntry *pml1_pte = &pml1->pte[pml1_index];
    pml1_pte->page_table_entry = ALIGN_DOWN(paddr, PAGE_SIZE) | flags | PTE_ATTR_P;
    return ptbase;

PML1_CRATE_FAILED:
    free_page(pml1);
PML2_CRATE_FAILED:
    free_page(pml2);
PML3_CRATE_FAILED:
    free_page(pml3);
PML4_CRATE_FAILED:
    return nullptr;
}

PageTable *init_pagetable(void)
{
    PageTable *pt = (PageTable *)alloc_page();
    if (!pt) return nullptr;
    memset(pt, 0, PAGE_SIZE);
    // 低地址线性映射
    for (uintptr_t laddr = 0; laddr < MB(16); laddr += PAGE_SIZE)
    {
        addr_mapping(pt, laddr, laddr, PTE_ATTR_P | PTE_ATTR_W);
    }
    // 内核空间线性映射区
    for (uintptr_t kladdr = 0; kladdr < MB(512); kladdr += PAGE_SIZE)
    {
        addr_mapping(pt, kladdr, kladdr + KERNEL_SPACE_BASE, PTE_ATTR_P | PTE_ATTR_W | PTE_ATTR_XD);
    }
    // 内核
    for (uintptr_t kcaddr = KERNEL_PHYS_BASE; kcaddr < MB(32); kcaddr += PAGE_SIZE)
    {
        addr_mapping(pt, kcaddr, kcaddr + KERNEL_VIRT_BASE - KERNEL_PHYS_BASE, PTE_ATTR_P | PTE_ATTR_W);
    }
    asm("mov cr3, %0" : : "r"(kvatopa(pt)));
    return pt;
}
