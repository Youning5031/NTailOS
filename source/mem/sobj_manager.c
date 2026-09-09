#include "sobj_manager.h"

#define NEED_MEM_UINTS
#include "buddy.h"
#include "clib/bits.h"
#include "clib/list.h"
#include "clib/string.h"
#include "clib/unit.h"
#include "memory.h"
#include "phymem.h"

static SobjManager *sobj_manager;

static SobjManager *sobj_manager_size_cache[SOBJ_DEFAULT_SIZE_MAX_POWER + 1];

static char *standard_manager_names[] = {
    "kmalloc-8",   "kmalloc-16",  "kmalloc-32", "kmalloc-64", "kmalloc-128",
    "kmalloc-256", "kmalloc-512", "kmalloc-1k", "kmalloc-2k",
};

void init_new_pages(PageFrame *pages, uint32_t obj_size, uint32_t obj_number)
{
    pages->next_free_obj = (FreeObj *)pf2addr(pages);
    pages->free_obj_number = obj_number;
    FreeObj *current_free_obj = pages->next_free_obj;
    for (size_t i = 0; i < obj_number - 1; i++)
    {
        current_free_obj->next = (FreeObj *)((uint8_t *)current_free_obj + obj_size);
        current_free_obj = current_free_obj->next;
    }
    current_free_obj->next = nullptr;
}

void sobj_add_page(SobjManager *manager, PageFrame *page)
{
    list_push(&manager->page_list[PAGE_LIST_FREE], &page->page_frame_list);
    manager->free_page_number += 1;
    uint32_t new_obj_number = (page->alloc_number << PAGE_SHIFT) / manager->obj_size;
    manager->obj_number += new_obj_number;
    manager->free_obj_number += new_obj_number;
}

void init_sobj(SobjManager *manager, const char *name, uint32_t obj_size)
{
    memset(manager, 0, sizeof(SobjManager));
    manager->name = name;
    manager->obj_size = ALIGN_UP(obj_size, SOBJ_OBJ_SIZE_ALIGN);
    manager->obj_number = 0;
    manager->free_obj_number = 0;
    manager->free_page_number = 0;

    for (PageListType i = PAGE_LIST_START; i < PAGE_LIST_END; i++)
    {
        manager->page_list[i] = INIT_LIST_NODE(&manager->page_list[i]);
    }
}

void create_sobj_manager()
{
    uint64_t page_num = 1;
    uint32_t obj_size = ALIGN_UP(sizeof(SobjManager), SOBJ_OBJ_SIZE_ALIGN);
    uint32_t obj_number = (page_num << PAGE_SHIFT) / obj_size;

    sobj_manager = (SobjManager *)alloc_pages((uint16_t)page_num);
    if (!sobj_manager) HLT;

    PageFrame *page = addr2pf(sobj_manager);

    init_new_pages(page, obj_size, obj_number);

    page->next_free_obj = page->next_free_obj->next;
    page->free_obj_number--;
    init_sobj(sobj_manager, "SobjManager", obj_size);
    list_push(&sobj_manager->page_list[PAGE_LIST_USING], &page->page_frame_list);
    sobj_manager->obj_number = obj_number;
    sobj_manager->free_obj_number = page->free_obj_number;

    // 构造标准大小分配器
    for (size_t i = 0; i <= SOBJ_DEFAULT_SIZE_MAX_POWER; i++)
    {
        SobjManager *kmalloc_sobj = manager_alloc(sobj_manager);
        if (!kmalloc_sobj) HLT;
        init_sobj(kmalloc_sobj, standard_manager_names[i], 1U << (i + 3));
        sobj_manager_size_cache[i] = kmalloc_sobj;
    }
}

void *manager_alloc(SobjManager *manager)
{
    FreeObj *obj = nullptr;
    lock(&manager->lock);

    if (manager->free_obj_number < SOBJ_MIN_OBJ_NUMBER)
    {
        FreeObj *new_pages = (FreeObj *)alloc_pages(PAGE_NUM(manager->obj_size * SOBJ_MIN_OBJ_NUMBER));
        if (!new_pages) goto ALLOC_PAGE_FAILED;
        PageFrame *pages = addr2pf(new_pages);
        init_new_pages(pages, manager->obj_size, SOBJ_MIN_OBJ_NUMBER);
        sobj_add_page(manager, pages);
    }

    if (list_is_empty(&manager->page_list[PAGE_LIST_USING]))
    {
        ListNode *free_page = list_pop(&manager->page_list[PAGE_LIST_FREE]);
        list_push(&manager->page_list[PAGE_LIST_USING], free_page);
        manager->free_page_number--;
    }

    ListNode  *page_node = manager->page_list[PAGE_LIST_USING].next;
    PageFrame *page = entryof(page_node, PageFrame, page_frame_list);
    obj = page->next_free_obj;
    page->next_free_obj = obj->next;
    manager->free_obj_number--;

    if (!page->next_free_obj)
    {
        list_delete(&page->page_frame_list);
        list_push(&manager->page_list[PAGE_LIST_FULL], &page->page_frame_list);
    }

ALLOC_PAGE_FAILED:
    unlock(&manager->lock);
    return obj;
}

void *sobj_malloc(size_t size)
{
    if (size == 0) return nullptr;

    // 计算指数，大小至少为 8*2^power
    size_t power = size > 8 ? sizeof(size_t) * 8 - clz(size) - 3 : 0;
    if (power <= SOBJ_DEFAULT_SIZE_MAX_POWER)
    {
        SobjManager *manager = sobj_manager_size_cache[power];
        return manager_alloc(manager);
    }
    else
    {
        return alloc_pages(PAGE_NUM(size));
    }
}

void sobj_free(void *addr)
{
    PageFrame *pages = addr2pf(addr);
    ;
    SobjManager *manager = pages->sobj_manager;

    lock(&manager->lock);
    pages->free_obj_number++;
    ((FreeObj *)addr)->next = pages->next_free_obj;
    pages->next_free_obj = addr;
    if (pages->free_obj_number == 1)
    {  // 归还后页框不满了，移入使用中链表
        list_delete(&pages->page_frame_list);
        list_push(&manager->page_list[PAGE_LIST_USING], &pages->page_frame_list);
    }
    else if (pages->alloc_number * KB(4) / manager->obj_size == pages->free_obj_number)
    {  // 归还之后页框空了，移入空闲链表
        list_delete(&pages->page_frame_list);
        list_push(&manager->page_list[PAGE_LIST_FREE], &pages->page_frame_list);
        manager->free_page_number++;
    }
    unlock(&manager->lock);
}
