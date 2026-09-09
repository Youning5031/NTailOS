#pragma once

#include "clib/list.h"
#include "clib/lock.h"
#include "phymem.h"

#include <stdint.h>

typedef struct s_sobj     SobjManager;
typedef struct s_free_obj FreeObj;

typedef struct s_page_frame
{
    uint64_t flags : 58;       // 标志位
    uint64_t zone  : 3;        // 内存区域编号
    uint64_t node  : 3;        // 内存节点编号
    ListNode page_frame_list;  // 页帧链表，在不同系统中会链接到不同的链表中
    union
    {
        // buddy
        struct
        {
            uint8_t order;  // buddy阶数
        };
        // 已分配
        struct
        {
            uint16_t     ref_cnt;          // 引用计数
            uint16_t     alloc_number;     // 分配的页帧数量
            uint16_t     free_obj_number;  // 空闲对象数量
            SobjManager *sobj_manager;     // 管理该页帧的sobj分配器
            FreeObj     *next_free_obj;    // 下一个可用的对象指针
        };
    };
    uint8_t _padding[16];  // 填充
} PageFrame;

static_assert(PAGE_SIZE % sizeof(PageFrame) == 0, "The size of PageFrame must divide the PAGE_SIZE");

#define PGF_HEAD          (1ULL << 0)
#define PFG_IS_HEAD(page) (page->flags & 0x1ULL)
#define PGF_FREE          (1ULL << 1)
#define PFG_IS_FREE(page) ((page->flags >> 1) & 0x1ULL)

typedef enum e_mem_zone_type
{
    ZONE_TYPE_START,
    ZONE_DMA16 = ZONE_TYPE_START,
    ZONE_DMA32,
    ZONE_NORMAL,
    ZONE_MOVABLE,
    ZONE_DEVICE,
    ZONE_MAX_NUMBER,
} MemZoneType;

typedef struct s_mem_node MemNode;

typedef struct s_mem_zone_free_area
{
    ListNode free_list;
    uint64_t free_page_number;
} MemNodeFreeArea;

#define BUDDY_ORDER_NUMBER 11
#define BUDDY_MAX_ORDER    (BUDDY_ORDER_NUMBER - 1)
#define BUDDY_MAX_BLOCK    (1ULL << BUDDY_MAX_ORDER)

typedef struct s_mem_zone
{
    MemZoneType     zone_type;
    Spinlock        lock;
    uint64_t        start_pfn;
    uint64_t        spanned_pages;
    uint64_t        present_pages;
    MemNodeFreeArea free_area[BUDDY_ORDER_NUMBER];
    MemNode        *zone_node;
} MemZone;

typedef enum e_mem_zone_list_type
{
    // ZONE_LIST_FALLBACK,  // UMA模型无需 fallback
    ZONE_LIST_NOFALLBACK,
    ZONE_LIST_MAX_NUMBER,
} MemZoneListType;

typedef struct s_mem_node_zone_list
{
    MemZone    *zone;
    MemZoneType type;
} MemNodeZoneList;

typedef struct s_mem_node
{
    uint8_t          node_id;
    Spinlock         lock;
    uint64_t         start_pfn;
    uint64_t         spanned_pages;
    uint64_t         present_pages;
    MemZone          zones[ZONE_MAX_NUMBER];
    MemNodeZoneList *zone_lists[ZONE_LIST_MAX_NUMBER];
} MemNode;

#define NODE_MAX_NUMBER 1
#define this_mem_node() 0

extern PageFrame *vpage;

#define addr2pfn(addr) ((uint64_t)(kvatopa(addr)) >> PAGE_SHIFT)
#define pfn2addr(pfn)  (kpatova((uint64_t)(pfn) << PAGE_SHIFT))

#define addr2pf(addr) (&vpage[addr2pfn(addr)])
#define pf2addr(pf)   (pfn2addr(pf - vpage))

void init_buddy(void);

PageFrame *buddy_get_pages(uint16_t n);
void       buddy_return_pages_by_order(PageFrame *page_frame, uint8_t order);
void       buddy_return_pages(PageFrame *page_frame, uint16_t n);

void     add_pages_to_buddy(uint64_t pfn, uint8_t order);
uint64_t add_region_to_buddy(uintptr_t addr, uint64_t size);

void *buddy_alloc_pages(uint16_t n);
void  buddy_free_pages(void *addr, uint16_t n);
