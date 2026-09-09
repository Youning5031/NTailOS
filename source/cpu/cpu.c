#include "clib/asm/farop.h"
#include "clib/print.h"
#include "clib/string.h"
#include "core/init.h"
#include "cpuid.h"

CpuidCache cpuid_cache;

void print_cpu_feature()
{
    kprintln();
    char vendor_id[13];
    ((uint32_t *)vendor_id)[0] = cpuid(&cpuid_cache, 0x0).cpu_vendorid_0;
    ((uint32_t *)vendor_id)[1] = cpuid(&cpuid_cache, 0x0).cpu_vendorid_1;
    ((uint32_t *)vendor_id)[2] = cpuid(&cpuid_cache, 0x0).cpu_vendorid_2;
    vendor_id[12] = '\0';
    kprintln("Vendor ID: %s", vendor_id);
    kprintln("Max Leaf: 0x%x", cpuid(&cpuid_cache, 0x0).max_std_leaf);
    kprintln("Max Extended Leaf: 0x%x", cpuid(&cpuid_cache, 0x80000000).max_ext_leaf);
    kprintln("Stepping ID: %d", cpuid(&cpuid_cache, 0x1).stepping);
    kprintln("Model ID: %d", cpuid(&cpuid_cache, 0x1).base_model);
    kprintln("Family ID: %d", cpuid(&cpuid_cache, 0x1).base_family_id);
    kprintln("Processor Type: %d", cpuid(&cpuid_cache, 0x1).cpu_type);
    kprintln("Extended Model ID: %d", cpuid(&cpuid_cache, 0x1).ext_model);
    kprintln("Extended Family ID: %d", cpuid(&cpuid_cache, 0x1).ext_family);
    kprintln("Brand Index: %d", cpuid(&cpuid_cache, 0x1).brand_id);
    kprintln("CLFLUSH Line Size: %d", cpuid(&cpuid_cache, 0x1).clflush_size);
    kprintln("Logical CPU count/APIC ID Space: %d", cpuid(&cpuid_cache, 0x1).n_logical_cpu);
    kprintln("Initial APIC ID: %d", cpuid(&cpuid_cache, 0x1).local_apic_id);
    kprintln("Physical Address Size: %d", cpuid(&cpuid_cache, 0x80000008).phys_addr_bits);
    kprintln("Virtual Address Size: %d", cpuid(&cpuid_cache, 0x80000008).virt_addr_bits);
    kprintln("Guest Physical Address Size: %d", cpuid(&cpuid_cache, 0x80000008).guest_phys_addr_bits);

    kprintln("APIC: %s", cpuid(&cpuid_cache, 0x1).apic ? "Yes" : "No");
    kprintln("x2APIC: %s", cpuid(&cpuid_cache, 0x1).x2apic ? "Yes" : "No");

    kprintln("SSE: %s", cpuid(&cpuid_cache, 0x1).sse ? "Yes" : "No");
    kprintln("SSE2: %s", cpuid(&cpuid_cache, 0x1).sse2 ? "Yes" : "No");
    kprintln("SSE3: %s", cpuid(&cpuid_cache, 0x1).sse3 ? "Yes" : "No");
    kprintln("SSSE3: %s", cpuid(&cpuid_cache, 0x1).ssse3 ? "Yes" : "No");
    kprintln("SSE4.1: %s", cpuid(&cpuid_cache, 0x1).sse4_1 ? "Yes" : "No");
    kprintln("SSE4.2: %s", cpuid(&cpuid_cache, 0x1).sse4_2 ? "Yes" : "No");
    kprintln("XSAVE: %s", cpuid(&cpuid_cache, 0x1).xsave ? "Yes" : "No");
    kprintln("AVX: %s", cpuid(&cpuid_cache, 0x1).avx ? "Yes" : "No");
    kprintln("RDRAND: %s", cpuid(&cpuid_cache, 0x1).rdrand ? "Yes" : "No");
    kprintln("FXSAVE & FXRSTOR: %s", cpuid(&cpuid_cache, 0x1).fxsr ? "Yes" : "No");

    kprintln();
}

INIT(cpu_feature)
{
    CpuidCache *pcpuid_cache = far_var_ptr(cpuid_cache);

    // clang-format off
#define fill_cpuid_cache(_pcache, _leaf)                                                                        \
    __cpuid(                                                                                                    \
        _leaf,                                                                                                  \
        cpuid_raw(_pcache, _leaf, eax), cpuid_raw(_pcache, _leaf, ebx),                                         \
        cpuid_raw(_pcache, _leaf, ecx), cpuid_raw(_pcache, _leaf, edx))
#define fill_cpuid_subleaf_cache(_pcache, _leaf, _subleaf)                                                      \
    __cpuid_count(                                                                                              \
        _leaf, _subleaf,                                                                                        \
        cpuid_subleaf_raw(_pcache, _leaf, _subleaf, eax), cpuid_subleaf_raw(_pcache, _leaf, _subleaf, ebx),     \
        cpuid_subleaf_raw(_pcache, _leaf, _subleaf, ecx), cpuid_subleaf_raw(_pcache, _leaf, _subleaf, edx))
#define _fill_cpuid_subleaf_n_cache(_pcache, _leaf, _subleaf)                                                   \
    __cpuid_count(                                                                                              \
        _leaf, _subleaf,                                                                                        \
        cpuid_subleaf_n_raw(_pcache, _leaf, _subleaf, eax), cpuid_subleaf_n_raw(_pcache, _leaf, _subleaf, ebx), \
        cpuid_subleaf_n_raw(_pcache, _leaf, _subleaf, ecx), cpuid_subleaf_n_raw(_pcache, _leaf, _subleaf, edx))
#define fill_cpuid_subleaf_n_cache(_pcache, _leaf)                                                              \
    for (int _i = SUBLEAF_N_FIRST(_leaf); _i <= SUBLEAF_N_LAST(_leaf); _i++)                                    \
    {                                                                                                           \
        _fill_cpuid_subleaf_n_cache(_pcache, _leaf, _i);                                                        \
    }

    // 基本功能页
    fill_cpuid_cache(pcpuid_cache, 0x0);
    switch (cpuid(pcpuid_cache, 0x0).max_std_leaf)
    {
    default: case 0x16:
        fill_cpuid_cache(pcpuid_cache, 0x16);
        FALLTHROUGH;
    case 0x15:
        fill_cpuid_cache(pcpuid_cache, 0x15);
        FALLTHROUGH;
    case 0x14: case 0x13: case 0x12: case 0x11:
    case 0x10: case 0xF:  case 0xE:  case 0xD:
        fill_cpuid_subleaf_cache(pcpuid_cache, 0xD, 0);
        fill_cpuid_subleaf_cache(pcpuid_cache, 0xD, 1);
        fill_cpuid_subleaf_n_cache(pcpuid_cache, 0xD);
        FALLTHROUGH;
    case 0xC: case 0xB:
        fill_cpuid_subleaf_n_cache(pcpuid_cache, 0xB);
        FALLTHROUGH;
    case 0xA: case 0x9: case 0x8: case 0x7:
        fill_cpuid_subleaf_cache(pcpuid_cache, 0x7, 0);
        fill_cpuid_subleaf_cache(pcpuid_cache, 0x7, 1);
        fill_cpuid_subleaf_cache(pcpuid_cache, 0x7, 2);
        FALLTHROUGH;
    case 0x6:
        fill_cpuid_cache(pcpuid_cache, 0x6);
        FALLTHROUGH;
    case 0x5:
        fill_cpuid_cache(pcpuid_cache, 0x5);
        FALLTHROUGH;
    case 0x4:
        fill_cpuid_subleaf_n_cache(pcpuid_cache, 0x4);
        FALLTHROUGH;
    case 0x3: case 0x2: case 0x1:
        fill_cpuid_cache(pcpuid_cache, 0x1);
    }

    // 扩展功能页
    fill_cpuid_cache(pcpuid_cache, 0x80000000);
    switch(cpuid(pcpuid_cache, 0x80000000).max_ext_leaf)
    {
        default: case 0x8000001F:
            fill_cpuid_cache(pcpuid_cache, 0x8000001F);
            FALLTHROUGH;
        case 0x8000001E:
            fill_cpuid_cache(pcpuid_cache, 0x8000001E);
            FALLTHROUGH;
        case 0x8000001D:
            fill_cpuid_subleaf_n_cache(pcpuid_cache, 0x8000001D);
            FALLTHROUGH;
        case 0x8000001C: case 0x8000001B: case 0x8000001A: case 0x80000019:
        case 0x80000018: case 0x80000017: case 0x80000016: case 0x80000015:
        case 0x80000014: case 0x80000013: case 0x80000012: case 0x80000011:
        case 0x80000010: case 0x8000000F: case 0x8000000E: case 0x8000000D:
        case 0x8000000C: case 0x8000000B: case 0x8000000A:
            fill_cpuid_cache(pcpuid_cache, 0x8000000A);
            FALLTHROUGH;
        case 0x80000009: case 0x80000008:
            fill_cpuid_cache(pcpuid_cache, 0x80000008);
            FALLTHROUGH;
        case 0x80000007:
            fill_cpuid_cache(pcpuid_cache, 0x80000007);
            FALLTHROUGH;
        case 0x80000006:
            fill_cpuid_cache(pcpuid_cache, 0x80000006);
            FALLTHROUGH;
        case 0x80000005:
            fill_cpuid_cache(pcpuid_cache, 0x80000005);
            FALLTHROUGH;
        case 0x80000004:
            fill_cpuid_cache(pcpuid_cache, 0x80000004);
            FALLTHROUGH;
        case 0x80000003:
            fill_cpuid_cache(pcpuid_cache, 0x80000003);
            FALLTHROUGH;
        case 0x80000002:
            fill_cpuid_cache(pcpuid_cache, 0x80000002);
            FALLTHROUGH;
        case 0x80000001:
            fill_cpuid_cache(pcpuid_cache, 0x80000001);
    }

#undef fill_cpuid_cache
#undef fill_cpuid_subleaf_cache
#undef _fill_cpuid_subleaf_n_cache
#undef fill_cpuid_subleaf_n_cache
    // clang-format on
    return true;
}
