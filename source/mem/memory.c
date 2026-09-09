#include "memory.h"

#include "boot/boot.h"
#include "buddy.h"
#include "clib/asm/farop.h"
#include "clib/logger.h"
#include "clib/string.h"
#include "core/init.h"
#include "interrupt/interrupt.h"
#include "mem_early.h"
#include "phymem.h"
#include "sobj_manager.h"

typedef struct s_memory_func
{
    void *(*_alloc_pages)(uint16_t);
    void (*_free_pages)(void *, uint16_t);
    void *(*_kmalloc)(size_t);
    void (*_kfree)(void *);
} MemoryFunc;

static MemoryFunc memory_func = {
    ._alloc_pages = &early_mem_alloc_pages,
    ._free_pages = &early_mem_free_pages,
    ._kmalloc = nullptr,
    ._kfree = nullptr,
};

INTERRUPT(page_fault_handler)
{
    uintptr_t cr2, cr3;
    asm("mov %0, cr3" : "=r"(cr3));
    asm("mov %0, cr2" : "=r"(cr2));

    kprintln("Page fault triggered!");
    kprintln("Address: 0x%lx:0x%lx", context->cs, context->rip);
    kprintln("CR3: 0x%lx, CR2: 0x%lx", cr3, cr2);

    uintptr_t mem = kvatopa(alloc_page());

    addr_mapping((PageTable *)kpatova(cr3 & ~0xFFFULL), mem, cr2, PTE_ATTR_P | PTE_ATTR_W | PTE_ATTR_XD);

    return;
}

INIT(memory)
{
    extern Bitmap    early_free_mem_bitmap;
    extern PageFrame _vpage_start[];

    far_store_val(vpage, far_arr_ptr(_vpage_start));

    PageTable *pt = far_call(init_pagetable);
    if (!pt) return false;

    struct multiboot_tag_mmap *mmap = nullptr;
    get_boot_info(boot_tags, 1, MULTIBOOT_TAG_TYPE_MMAP, &mmap);

    uint32_t entry_num = (mmap->size - sizeof(struct multiboot_tag)) / mmap->entry_size;
    for (uint32_t i = 0; i < entry_num; i++)
    {
        struct multiboot_mmap_entry *entry = &mmap->entries[i];
        if (entry->type != MULTIBOOT_MEMORY_AVAILABLE) continue;

        uint64_t start_pfn = ALIGN_UP(entry->addr, PAGE_SIZE) >> PAGE_SHIFT;
        uint64_t end_pfn = ALIGN_DOWN(entry->addr + entry->len, PAGE_SIZE) >> PAGE_SHIFT;
        uint64_t last_mapping_pfn = UINT64_MAX;
        for (size_t i = start_pfn; i < end_pfn; i++)
        {
            uint64_t need_mapping_pfn = ALIGN_DOWN(i, PAGE_SIZE / sizeof(PageFrame));
            if (need_mapping_pfn != last_mapping_pfn)
            {
                void *vp = far_call(alloc_pages, 1);
                if (!vp) return false;
                memset(vp, 0, PAGE_SIZE);
                far_call(
                    addr_mapping, pt, kvatopa(vp), (uintptr_t)&far_load_val(vpage)[i],
                    PTE_ATTR_P | PTE_ATTR_W | PTE_ATTR_XD);
                last_mapping_pfn = need_mapping_pfn;
            }
        }
    }

    init_buddy();
    uint64_t pfn = 0;
    size_t   size = 0;
    while (true)
    {
        pfn = far_call(bitmap_find_first_free_with_start, far_var_ptr(early_free_mem_bitmap), pfn + size);
        if (pfn == UINT64_MAX) break;
        size = far_call(bitmap_count_contiguous_free_size, far_var_ptr(early_free_mem_bitmap), pfn);
        far_call(add_region_to_buddy, pfn << PAGE_SHIFT, size << PAGE_SHIFT);
    }

    MemoryFunc *pmemory_func = far_var_ptr(memory_func);
    pmemory_func->_alloc_pages = &buddy_alloc_pages;
    pmemory_func->_free_pages = &buddy_free_pages;

    far_call(create_sobj_manager);
    pmemory_func->_kmalloc = &sobj_malloc;
    pmemory_func->_kfree = &sobj_free;

    far_call(register_interrupt_handler, INT_VEC_PF, page_fault_handler);

    return true;
}

patch_function(5) void *alloc_pages(uint16_t n)
{
    return memory_func._alloc_pages(n);
}

patch_function(5) void free_pages(void *addr, uint16_t n)
{
    memory_func._free_pages(addr, n);
    return;
}

patch_function(5) void *kmalloc(size_t size)
{
    return memory_func._kmalloc(size);
}

patch_function(5) void kfree(void *addr)
{
    memory_func._kfree(addr);
    return;
}

// /**
//  * @brief `memset`
//  * @param s 填充地址
//  * @param c 填充内容
//  * @param n 填充长度
//  * @return
//  */
// [[gnu::noinline]] void *memset(void *s, int c, unsigned long n)
// {
//     unsigned char *p = (unsigned char *)s;
//     while (n--)
//     {
//         *p = (unsigned char)c;
//         p++;
//     }
//     return s;
// }