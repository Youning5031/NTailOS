#include "mem_early.h"

#include "boot/boot.h"
#include "buddy.h"
#include "clib/bitmap.h"
#include "clib/print.h"
#include "core/init.h"
#include "elf.h"
#include "interrupt/interrupt.h"
#include "memory.h"
#include "multiboot2.h"
#include "phymem.h"

#define EARLY_MEM_SIZE MB(32) / PAGE_SIZE

BOOT_DATA Bitmap early_free_mem_bitmap = {
    .total_bits = EARLY_MEM_SIZE,
    .bits = {[0 ... EARLY_MEM_SIZE / 64 - 1] UINT64_MAX},
};

BOOT_DATA Bitmap early_used_mem_bitmap = {
    .total_bits = EARLY_MEM_SIZE,
    .bits = {[0 ... EARLY_MEM_SIZE / 64 - 1] UINT64_MAX},
};

INIT(early_mem)
{
    struct multiboot_tag_mmap         *mmap = nullptr;
    struct multiboot_tag_elf_sections *elf_sections = nullptr;

    get_boot_info(boot_tags, 2, MULTIBOOT_TAG_TYPE_MMAP, &mmap, MULTIBOOT_TAG_TYPE_ELF_SECTIONS, &elf_sections);
    if (mmap == nullptr || elf_sections == nullptr) return false;

    // 扫描可用内存
    for (uint32_t i = 0; i < (mmap->size - sizeof(struct multiboot_tag)) / mmap->entry_size; i++)
    {
        struct multiboot_mmap_entry *entry = &mmap->entries[i];
        if (entry->type != MULTIBOOT_MEMORY_AVAILABLE) continue;

        uint64_t start_pfn = ALIGN_UP(entry->addr, PAGE_SIZE) >> PAGE_SHIFT;
        if (start_pfn >= EARLY_MEM_SIZE) break;

        uint64_t end_pfn = ALIGN_DOWN(entry->addr + entry->len, PAGE_SIZE) >> PAGE_SHIFT;
        if (end_pfn > EARLY_MEM_SIZE) end_pfn = EARLY_MEM_SIZE;

        bitmap_clear(&early_free_mem_bitmap, start_pfn, end_pfn);
    }

    // 扫描内核占用
    auto sect = (Elf64_Shdr *)&elf_sections->sections;
    for (uint32_t i = 0; i < elf_sections->num; i++)
    {
        // 只留下SHF_ALLOC，其他节不需要
        if (!(sect[i].sh_flags & SHF_ALLOC)) continue;

        uintptr_t phy_addr = sect[i].sh_addr;
        if (phy_addr >= KERNEL_VIRT_BASE) phy_addr = phy_addr - KERNEL_VIRT_BASE + KERNEL_PHYS_BASE;

        uint64_t start_pfn = ALIGN_DOWN(phy_addr, PAGE_SIZE) >> PAGE_SHIFT;
        if (start_pfn >= EARLY_MEM_SIZE) break;

        uint64_t end_pfn = ALIGN_UP(phy_addr + sect[i].sh_size, PAGE_SIZE) >> PAGE_SHIFT;
        if (end_pfn > EARLY_MEM_SIZE) end_pfn = EARLY_MEM_SIZE;

        bitmap_set(&early_free_mem_bitmap, start_pfn, end_pfn);
        bitmap_clear(&early_used_mem_bitmap, start_pfn, end_pfn);
    }

    // // 扫描可用内存
    // for (uint32_t i = 0; i < (mmap->size - sizeof(struct multiboot_tag)) / mmap->entry_size; i++)
    // {
    //     struct multiboot_mmap_entry *entry = &mmap->entries[i];
    //     if (entry->type != MULTIBOOT_MEMORY_AVAILABLE) continue;

    //     uint64_t start_pfn = ALIGN_UP(entry->addr, PAGE_SIZE) >> PAGE_SHIFT;
    //     if (start_pfn >= EARLY_MEM_SIZE) break;

    //     uint64_t end_pfn = ALIGN_DOWN(entry->addr + entry->len, PAGE_SIZE) >> PAGE_SHIFT;
    //     if (end_pfn > EARLY_MEM_SIZE) end_pfn = EARLY_MEM_SIZE;

    //     bitmap_clear(&early_free_mem_bitmap, start_pfn, end_pfn);
    // }

    return true;
}

BOOT_CODE void *early_mem_alloc_pages(uint16_t n)
{
    if (n == 0) return nullptr;

    uint64_t pfn;
    if (n == 1)
    {
        pfn = bitmap_find_first_free_with_start(&early_free_mem_bitmap, MB(16) >> PAGE_SHIFT);
        if (pfn == UINT64_MAX) return nullptr;

        bitmap_bit_set(&early_free_mem_bitmap, pfn);
        bitmap_bit_clear(&early_used_mem_bitmap, pfn);
    }
    else
    {
        pfn = bitmap_find_contiguous_free_with_start(&early_free_mem_bitmap, MB(16) >> PAGE_SHIFT, n);
        if (pfn == UINT64_MAX) return nullptr;

        bitmap_set(&early_free_mem_bitmap, pfn, pfn + n);
        bitmap_clear(&early_used_mem_bitmap, pfn, pfn + n);
    }
    return (void *)kpatova(pfn << PAGE_SHIFT);
}

BOOT_CODE void early_mem_free_pages(void *addr, uint16_t n)
{
    if (addr == nullptr || n == 0) return;

    uint64_t pfn = kvatopa(addr) >> PAGE_SHIFT;
    if (n == 1)
    {
        if (bitmap_bit_test(&early_used_mem_bitmap, pfn)) return;

        bitmap_bit_clear(&early_free_mem_bitmap, pfn);
        bitmap_bit_set(&early_used_mem_bitmap, pfn);
    }
    else
    {
        if (bitmap_test(&early_used_mem_bitmap, pfn, pfn + n)) return;

        bitmap_clear(&early_free_mem_bitmap, pfn, pfn + n);
        bitmap_set(&early_used_mem_bitmap, pfn, pfn + n);
    }
}
