#include "clib/macro.h"
#include "cpuid_leaf_types_gen.h"

#include_next <cpuid.h>

#define SUBLEAF_N_LAST(_leaf)   CONCAT(CONCAT(LEAF, _leaf), SUBLEAF_N_LAST)
#define SUBLEAF_N_FIRST(_leaf)  CONCAT(CONCAT(LEAF, _leaf), SUBLEAF_N_FIRST)
#define SUBLEAF_N_NUMBER(_leaf) (SUBLEAF_N_LAST(_leaf) - SUBLEAF_N_FIRST(_leaf) + 1)

#define SUBLEAF(_leaf, _subleaf)   CONCAT(leaf, CONCAT(_leaf, _subleaf))
#define SUBLEAF_N(_leaf, _subleaf) SUBLEAF(_leaf, n)[_subleaf - SUBLEAF_N_FIRST(_leaf)]
#define LEAF(_leaf)                SUBLEAF(_leaf, 0)

#define SUBLEAF_TYPE(_leaf, _subleaf) CONCAT(Leaf, CONCAT(_leaf, _subleaf))
#define SUBLEAF_TYPE_N(_leaf)         SUBLEAF_TYPE(_leaf, n)
#define LEAF_TYPE(_leaf)              SUBLEAF_TYPE(_leaf, 0)

typedef struct
{
#define STRUCT_SUBLEAF_CACHE(_leaf, _subleaf) SUBLEAF_TYPE(_leaf, _subleaf) SUBLEAF(_leaf, _subleaf)
#define STRUCT_SUBLEAF_N_CACHE(_leaf)         STRUCT_SUBLEAF_CACHE(_leaf, n)[SUBLEAF_N_NUMBER(_leaf)]
#define STRUCT_LEAF_CACHE(_leaf)              STRUCT_SUBLEAF_CACHE(_leaf, 0)
    // 基本功能页
    STRUCT_LEAF_CACHE(0x0);
    STRUCT_LEAF_CACHE(0x1);
    STRUCT_SUBLEAF_N_CACHE(0x4);
    STRUCT_LEAF_CACHE(0x5);
    STRUCT_LEAF_CACHE(0x6);
    STRUCT_SUBLEAF_CACHE(0x7, 0);
    STRUCT_SUBLEAF_CACHE(0x7, 1);
    STRUCT_SUBLEAF_CACHE(0x7, 2);
    STRUCT_SUBLEAF_N_CACHE(0xB);
    STRUCT_SUBLEAF_CACHE(0xD, 0);
    STRUCT_SUBLEAF_CACHE(0xD, 1);
    STRUCT_SUBLEAF_N_CACHE(0xD);
    STRUCT_LEAF_CACHE(0x15);
    STRUCT_LEAF_CACHE(0x16);

    // 扩展功能页
    STRUCT_LEAF_CACHE(0x80000000);
    STRUCT_LEAF_CACHE(0x80000001);
    STRUCT_LEAF_CACHE(0x80000002);
    STRUCT_LEAF_CACHE(0x80000003);
    STRUCT_LEAF_CACHE(0x80000004);
    STRUCT_LEAF_CACHE(0x80000005);
    STRUCT_LEAF_CACHE(0x80000006);
    STRUCT_LEAF_CACHE(0x80000007);
    STRUCT_LEAF_CACHE(0x80000008);
    STRUCT_LEAF_CACHE(0x8000000A);
    STRUCT_SUBLEAF_N_CACHE(0x8000001D);
    STRUCT_LEAF_CACHE(0x8000001E);
    STRUCT_LEAF_CACHE(0x8000001F);

#undef STRUCT_SUBLEAF_CACHE
#undef STRUCT_SUBLEAF_N_CACHE
#undef STRUCT_LEAF_CACHE

} CpuidCache;

#define cpuid_subleaf(_pcache, _leaf, _subleaf)   (_pcache)->SUBLEAF(_leaf, _subleaf)
#define cpuid_subleaf_n(_pcache, _leaf, _subleaf) (_pcache)->SUBLEAF_N(_leaf, _subleaf)
#define cpuid(_pcache, _leaf)                     cpuid_subleaf(_pcache, _leaf, 0)

#define cpuid_subleaf_raw(_pcache, _leaf, _subleaf, reg)   cpuid_subleaf(_pcache, _leaf, _subleaf).reg
#define cpuid_subleaf_n_raw(_pcache, _leaf, _subleaf, reg) cpuid_subleaf_n(_pcache, _leaf, _subleaf).reg
#define cpuid_raw(_pcache, _leaf, reg)                     cpuid_subleaf_raw(_pcache, _leaf, 0, reg)

extern CpuidCache cpuid_cache;
