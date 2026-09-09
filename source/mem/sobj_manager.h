#pragma once

#include "buddy.h"
#include "clib/list.h"
#include "phymem.h"

#include <stdint.h>

typedef enum e_page_list_type
{
    PAGE_LIST_START,
    PAGE_LIST_USING = PAGE_LIST_START,
    PAGE_LIST_FREE,
    PAGE_LIST_FULL,
    PAGE_LIST_END,
} PageListType;

typedef struct s_sobj
{
    Spinlock    lock;
    // ListNode    sobj_list;                 // sobj链表
    const char *name;                      // 分配器名称
    uint32_t    obj_size;                  // 一个对象的大小，最小8字节，8字节对齐
    uint32_t    obj_number;                // 管理的对象总数
    uint32_t    free_obj_number;           // 空闲对象总数
    uint32_t    free_page_number;          // 空闲页帧数
    ListNode    page_list[PAGE_LIST_END];  // 页帧链表数组
} SobjManager;

typedef struct s_free_obj FreeObj;

struct s_free_obj
{
    FreeObj *next;
};

#define SOBJ_OBJ_SIZE_ALIGN 8

#define SOBJ_DEFAULT_SIZE_MAX_POWER 8  // 8*2^8=2048字节

#define SOBJ_MIN_OBJ_NUMBER 4  // 4只是随便填的

void create_sobj_manager(void);

void init_new_pages(PageFrame *pages, uint32_t obj_size, uint32_t obj_number);

void *manager_alloc(SobjManager *manager);
void *sobj_malloc(size_t size);
void  manager_free(SobjManager *manager, void *addr);
void  sobj_free(void *addr);