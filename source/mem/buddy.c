#include "buddy.h"

#include "boot/boot.h"
#include "clib/asm/farop.h"
#include "clib/assert.h"
#include "clib/bits.h"
#include "clib/math.h"
#include "clib/string.h"
#include "memory.h"
#include "phymem.h"

extern uint8_t _vpage_start[];
extern uint8_t _vpage_end[];

PageFrame *vpage;

static MemNode *mem_node[NODE_MAX_NUMBER];
static uint8_t  mem_node_number;

BOOT_CODE void init_buddy(void)
{
    size_t   buddy_size = sizeof(MemNode) + sizeof(MemNodeZoneList) * (ZONE_MAX_NUMBER);
    MemNode *node = (MemNode *)far_call(alloc_pages, PAGE_NUM(buddy_size));
    memset(node, 0, buddy_size);

    for (size_t i = ZONE_TYPE_START; i < ZONE_MAX_NUMBER; i++)
    {
        node->zones[i].zone_type = i;
        MemNodeFreeArea *area = node->zones[i].free_area;
        for (size_t j = 0; j < BUDDY_ORDER_NUMBER; j++)
        {
            area[j].free_list = INIT_LIST_NODE(&area[j].free_list);
        }
    }

    MemNodeZoneList *zone_list = (MemNodeZoneList *)(node + 1);

    node->zone_lists[ZONE_LIST_NOFALLBACK] = zone_list;

    zone_list[0].zone = &node->zones[ZONE_NORMAL];
    zone_list[0].type = ZONE_NORMAL;

    zone_list[1].zone = &node->zones[ZONE_MOVABLE];
    zone_list[1].type = ZONE_MOVABLE;

    zone_list[2].zone = &node->zones[ZONE_DMA32];
    zone_list[2].type = ZONE_DMA32;

    zone_list[3].zone = &node->zones[ZONE_DMA16];
    zone_list[3].type = ZONE_DMA16;

    zone_list[4].zone = nullptr;
    zone_list[4].type = ZONE_MAX_NUMBER;

    far_arr_ptr(mem_node)[this_mem_node()] = node;
    far_store_val(mem_node_number, 1);

    return;
}

MemZone *get_available_zone(MemNode *node, uint8_t *order, MemZoneListType can_fallback)
{
    MemNodeZoneList *zone_list = node->zone_lists[can_fallback];
    for (size_t i = 0; zone_list[i].zone; i++)
    {
        MemZone *zone = zone_list[i].zone;
        for (uint8_t j = *order; j < BUDDY_MAX_ORDER; j++)
        {
            if (zone->free_area[j].free_page_number)
            {
                *order = j;
                return zone;
            }
        }
    }
    return nullptr;
}

void init_page_for_free(PageFrame *page, uint8_t order)
{
    for (size_t i = 0; i < 1ULL << order; i++)
    {
        page[i].page_frame_list = INIT_LIST_NODE(&page[i].page_frame_list);
        page[i].order = order;
        page[i].flags = PGF_FREE;
    }
    page->flags = PGF_FREE | PGF_HEAD;
    return;
}

void buddy_return_pages_by_order(PageFrame *page, uint8_t order)
{
    if (!page) return;
    MemZone *zone = &mem_node[page->node]->zones[page->zone];

    init_page_for_free(page, order);
    list_push(&zone->free_area[order].free_list, &page->page_frame_list);
    zone->free_area[order].free_page_number += 1ULL << order;

    return;
}

void buddy_return_pages(PageFrame *page, uint16_t n)
{
    if (!page) return;
    uint64_t pfn = page - vpage;
    while (n > 0)
    {
        uint8_t order = max(ctz(pfn), BUDDY_MAX_ORDER);
        while (1U << order > n) order--;

        buddy_return_pages_by_order(&vpage[pfn], order);
        pfn += 1ULL << order;
        n -= 1ULL << order;
    }
    return;
}

void init_page_for_use(PageFrame *page, uint16_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        page[i].alloc_number = n;
        page[i].flags = 0;
        page[i].ref_cnt = 1;
    }
    page->flags = PGF_HEAD;
    return;
}

PageFrame *buddy_get_pages(uint16_t n)
{
    if (n == 0 || n > BUDDY_MAX_BLOCK) return nullptr;

    uint8_t order = bsr(n);
    if (1U << order != n) order++;

    MemZone *zone = get_available_zone(mem_node[this_mem_node()], &order, ZONE_LIST_NOFALLBACK);
    if (!zone) return nullptr;

    ListNode *page_node = list_pop(&zone->free_area[order].free_list);
    zone->free_area[order].free_page_number -= 1ULL << order;
    PageFrame *page = entryof(page_node, PageFrame, page_frame_list);

    assert(
        (uintptr_t)page >= (uintptr_t)vpage && (uintptr_t)page < (uintptr_t)vpage + VPAGE_NUMBER * sizeof(PageFrame),
        "page = 0x%lx, vpage = 0x%lx, page out of vpage range!", page, vpage);

    init_page_for_use(page, n);

    buddy_return_pages(page + n, (1U << order) - n);

    return page;
}

void add_pages_to_buddy(uint64_t pfn, uint8_t order)
{
    MemZoneType zone_type;
    if (pfn < MB(16) / PAGE_SIZE) zone_type = ZONE_DMA16;
    else if (pfn < GB(4) / PAGE_SIZE) zone_type = ZONE_DMA32;
    else zone_type = ZONE_NORMAL;

    MemNode *node = mem_node[this_mem_node()];
    uint64_t new_page_count = 1U << order;

    lock(&node->lock);
    MemZone *zone = &node->zones[zone_type];

    lock(&zone->lock);
    MemNodeFreeArea *area = &zone->free_area[order];
    PageFrame       *page = &vpage[pfn];
    init_page_for_free(page, order);
    list_insert_prev(&page->page_frame_list, &area->free_list);
    area->free_page_number += new_page_count;

    if (zone->start_pfn > pfn) zone->start_pfn = pfn;
    zone->present_pages += new_page_count;
    if (zone->spanned_pages < pfn + new_page_count - zone->start_pfn)
        zone->spanned_pages = pfn + new_page_count - zone->start_pfn;
    unlock(&zone->lock);

    if (node->start_pfn > pfn) node->start_pfn = pfn;
    node->present_pages += new_page_count;
    if (node->spanned_pages < pfn + new_page_count - node->start_pfn)
        node->spanned_pages = pfn + new_page_count - node->start_pfn;
    unlock(&node->lock);

    return;
}

uint64_t add_region_to_buddy(uintptr_t addr, uint64_t size)
{
    uint64_t start_pfn = ALIGN_UP(addr, PAGE_SIZE) >> PAGE_SHIFT;
    uint64_t end_pfn = ALIGN_DOWN(addr + size, PAGE_SIZE) >> PAGE_SHIFT;
    uint64_t pages = end_pfn - start_pfn;

    uint64_t pfn = start_pfn;
    uint64_t _pages = pages;
    while (_pages > 0)
    {
        uint8_t order = min(ctz(pfn), BUDDY_MAX_ORDER);
        while (1ULL << order > _pages)
        {
            order--;
        }
        add_pages_to_buddy(pfn, order);
        pfn += 1ULL << order;
        _pages -= 1ULL << order;
    }

    return pages;
}

void *buddy_alloc_pages(uint16_t n)
{
    if (n == 0) return nullptr;
    PageFrame *page = buddy_get_pages(n);
    return pf2addr(page);
}

void buddy_free_pages(void *addr, uint16_t n)
{
    if (n == 0 || addr == nullptr) return;
    uint64_t pfn = kvatopa(addr) >> PAGE_SHIFT;
    buddy_return_pages(&vpage[pfn], n);
}
