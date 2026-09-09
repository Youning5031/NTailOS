#pragma once

#define NEED_MEM_UINTS
#include "clib/unit.h"

#if !defined(ASM_FILE) && !defined(LD_FILE)

#  include "clib/list.h"
#  include "clib/lock.h"
#  include "memory.h"

#  include <stdint.h>

// paging

// Present 存在
#  define PTE_ATTR_P      (1ULL << 0)
// Write 可写
#  define PTE_ATTR_W      (1ULL << 1)
// User 用户态
#  define PTE_ATTR_U      (1ULL << 2)
// Write-Through 写透
#  define PTE_ATTR_PWT    (1ULL << 3)
// Cache Disable 禁用缓存
#  define PTE_ATTR_PCD    (1ULL << 4)
// Accessed 已访问
#  define PTE_ATTR_A      (1ULL << 5)
// Dirty 脏页
#  define PTE_ATTR_D      (1ULL << 6)
// Page Size 大页
#  define PTE_ATTR_PS     (1ULL << 7)
// Global 全局页
#  define PTE_ATTR_G      (1ULL << 8)
// Execute Disable 禁止执行
#  define PTE_ATTR_XD     (1ULL << 63)
// Protection Key 保护密钥
#  define PTE_ATTR_PK(pk) (((pk) & 0xFFFFULL) << 59)

// Common Page Address 下一级页表索引地址 [12:MPAB]
#  define PTE_SET_ADDR(addr)     ((addr) & SET_BITS_U64(12, LINEAR_ADDRESS_SIZE))
// 1GB Page Address 1GB大页物理地址 [30:MPAB]
#  define PTE_SET_1GB_ADDR(addr) ((addr) & SET_BITS_U64(30, LINEAR_ADDRESS_SIZE))
// 2MB Page Address 2MB大页物理地址 [30:MPAB]
#  define PTE_SET_2MB_ADDR(addr) ((addr) & SET_BITS_U64(21, LINEAR_ADDRESS_SIZE))

#  define ALIGN_UP(addr, n)     \
      ({                        \
        uintptr_t _addr = addr; \
        uintptr_t _n = (n) - 1; \
        (_addr + _n) & ~_n;     \
      })

#  define ALIGN_DOWN(addr, n)   \
      ({                        \
        uintptr_t _addr = addr; \
        uintptr_t _n = (n) - 1; \
        _addr & ~_n;            \
      })

typedef union s_page_table_entry
{
    struct PACKED
    {
        // 低位属性
        uint64_t present         : 1;  // P      - 存在位
        uint64_t writable        : 1;  // R/W    - 可写
        uint64_t user_accessible : 1;  // U/S    - 用户模式可访问
        uint64_t write_through   : 1;  // PWT    - 直写缓存
        uint64_t cache_disabled  : 1;  // PCD    - 禁用缓存
        uint64_t accessed        : 1;  // A      - 已访问
        uint64_t dirty           : 1;  // D      - 脏页（PML1、PML2.PS=1、PML3.PS=1可用），否则忽略
        uint64_t pat_ps          : 1;  // PAT PS - 页属性表（PML1可用），否则为PS位
        uint64_t global          : 1;  // G      - 全局页，不刷新TLB（PML1、PML2.PS=1、PML3.PS=1可用），否则忽略
        uint64_t ignored_0       : 3;  // IGN    - OS可用

        // 地址
        uint64_t addr : LINEAR_ADDRESS_SIZE - 12;
        uint64_t reserved : 52 - LINEAR_ADDRESS_SIZE;

        // 高位属性
        uint64_t ignored_1   : 7;
        uint64_t protect_key : 4;
        uint64_t xd          : 1;  // XD/NX - 禁止执行
    };
    uint64_t page_table_entry;
} PageTableEntry;

typedef struct s_page_table
{
    PageTableEntry pte[512];
} PageTable;

typedef struct mem_descriptor
{
    PageTable *paging_table;
    uint64_t   ref_cnt;
} MemDescriptor;

PageTable *init_pagetable(void);
PageTable *addr_mapping(PageTable *ptbase, uintptr_t paddr, uintptr_t vaddr, uint64_t flags);

#else  // ASM_FILE LD_FILE

// Maximum Linear Addresses Bit 最大线性地址宽度
#  define LINEAR_ADDRESS_SIZE 48

#  define MAX_PHY_MEM_SIZE  TB(64)
#  define PAGE_SIZE         KB(4)
#  define PAGE_SHIFT        12
#  define BOOT_PHYS_BASE    MB(1)
#  define KERNEL_PHYS_BASE  MB(2)
#  define KERNEL_SPACE_BASE 0xFFFF800000000000
#  define VPAGE_BASE        0xFFFF900000000000
#  define VPAGE_NUMBER      MAX_PHY_MEM_SIZE / PAGE_SIZE
#  define KERNEL_VIRT_BASE  0xFFFFFFFF80000000

// Present 存在
#  define PTE_ATTR_P        (1 << 0)
// Write 可写
#  define PTE_ATTR_W        (1 << 1)
// User 用户态
#  define PTE_ATTR_U        (1 << 2)
// Write-Through 写透
#  define PTE_ATTR_PWT      (1 << 3)
// Cache Disable 禁用缓存
#  define PTE_ATTR_PCD      (1 << 4)
// Accessed 已访问
#  define PTE_ATTR_A        (1 << 5)
// Dirty 脏页
#  define PTE_ATTR_D        (1 << 6)
// Page Size 大页
#  define PTE_ATTR_PS       (1 << 7)
// Global 全局页
#  define PTE_ATTR_G        (1 << 8)
// Execute Disable 禁止执行
#  define PTE_ATTR_XD       (1 << 63)

#endif  // ASM_FILE LD_FILE