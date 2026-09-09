#pragma once

#ifndef ASM_FILE
#  include <stdint.h>
INLINE uint64_t readmsr(uint32_t msr)
{
    uint32_t low, high;
    asm("rdmsr\n\t" : "=a"(low), "=d"(high) : "c"(msr));
    return ((uint64_t)high << 32) | low;
}

INLINE uint32_t readmsr32(uint32_t msr)
{
    uint32_t low;
    asm("rdmsr\n\t" : "=a"(low) : "c"(msr) : "rdx");
    return low;
}

INLINE void writemsr(uint32_t msr, uint64_t value)
{
    uint32_t low = (uint32_t)value, high = (uint32_t)(value >> UINT32_WIDTH);
    asm("wrmsr\n\t" ::"c"(msr), "a"(low), "d"(high):"memory");
    return;
}

INLINE void writemsr32(uint32_t msr, uint32_t value)
{
    asm("wrmsr\n\t" ::"c"(msr), "a"(value), "d"(0x0):"memory");
    return;
}

#endif

#define IA32_P5_MC_ADDR               0x0U    // 0 P5_MC_ADDR
#define IA32_P5_MC_TYPE               0x1U    // 1 P5_MC_TYPE
#define IA32_MONITOR_FILTER_SIZE      0x6U    // 6
#define IA32_TIME_STAMP_COUNTER       0x10U   // 16 TSC
#define IA32_PLATFORM_ID              0x17U   // 23 MSR_PLATFORM_ID
#define IA32_APIC_BASE                0x1BU   // 27 APIC_BASE
#define IA32_BARRIER                  0x2FU   // 47
#define IA32_FEATURE_CONTROL          0x3AU   // 58
#define IA32_TSC_ADJUST               0x3BU   // 59
#define IA32_SPEC_CTRL                0x48U   // 72
#define IA32_PRED_CMD                 0x49U   // 73
#define IA32_PPIN_CTL                 0x4EU   // 78
#define IA32_PPIN                     0x4FU   // 79
#define IA32_BIOS_UPDT_TRIG           0x79U   // 121 BIOS_UPDT_TRIG
#define IA32_FEATURE_ACTIVATION       0x7AU   // 122
#define IA32_MCU_ENUMERATION          0x7BU   // 123
#define IA32_MCU_STATUS               0x7CU   // 124
#define IA32_FZM_RANGE_INDEX          0x82U   // 130
#define IA32_FZM_DOMAIN_CONFIG        0x83U   // 131
#define IA32_FZM_RANGE_STARTADDR      0x84U   // 132
#define IA32_FZM_RANGE_ENDADDR        0x85U   // 133
#define IA32_FZM_RANGE_WRITESTATUS    0x86U   // 134
#define IA32_MKTME_KEYID_PARTITIONING 0x87U   // 135
#define IA32_BIOS_SIGN_ID             0x8BU   // 139 BIOS_SIGN/BBL_CR_D3
#define IA32_SGXLEPUBKEYHASH0         0x8CU   // 140
#define IA32_SGXLEPUBKEYHASH1         0x8DU   // 141
#define IA32_SGXLEPUBKEYHASH2         0x8EU   // 142
#define IA32_SGXLEPUBKEYHASH3         0x8FU   // 143
#define IA32_SGXLEPUBKEYHASH4         0x90U   // 144
#define IA32_SGXLEPUBKEYHASH5         0x91U   // 145
#define IA32_SMM_MONITOR_CTL          0x9BU   // 155
#define IA32_SMBASE                   0x9EU   // 158
#define IA32_MISC_PACKAGE_CTLS        0xBCU   // 188
#define IA32_XAPIC_DISABLE_STATUS     0xBDU   // 189
#define IA32_PMC0                     0xC1U   // 193 PERFCTR0
#define IA32_PMC1                     0xC2U   // 194 PERFCTR1
#define IA32_PMC2                     0xC3U   // 195
#define IA32_PMC3                     0xC4U   // 196
#define IA32_PMC4                     0xC5U   // 197
#define IA32_PMC5                     0xC6U   // 198
#define IA32_PMC6                     0xC7U   // 199
#define IA32_PMC7                     0xC8U   // 200
#define IA32_PMC8                     0xC9U   // 201
#define IA32_PMC9                     0xCAU   // 202
#define IA32_CORE_CAPABILITIES        0xCFU   // 207
#define IA32_UMWAIT_CONTROL           0xE1U   // 225
#define IA32_MPERF                    0xE7U   // 231
#define IA32_APERF                    0xE8U   // 232
#define IA32_MTRRCAP                  0xFEU   // 254 MTRRcap
#define IA32_ARCH_CAPABILITIES        0x10AU  // 266
#define IA32_FLUSH_CMD                0x10BU  // 267
#define IA32_TSX_FORCE_ABORT          0x10FU  // 271
#define IA32_TSX_CTRL                 0x122U  // 290
#define IA32_MCU_OPT_CTRL             0x123U  // 291
#define IA32_SYSENTER_CS              0x174U  // 372
#define IA32_SYSENTER_ESP             0x175U  // 373
#define IA32_SYSENTER_EIP             0x176U  // 374
#define IA32_MCG_CAP                  0x179U  // 377 MCG_CAP
#define IA32_MCG_STATUS               0x17AU  // 378 MCG_STATUS
#define IA32_MCG_CTL                  0x17BU  // 379 MCG_CTL
// 180H−185H, 384−389 N/A
#define IA32_PERFEVTSEL0              0x186U  // 390 PERFEVTSEL0
#define IA32_PERFEVTSEL1              0x187U  // 391 PERFEVTSEL1
#define IA32_PERFEVTSEL2              0x188U  // 392
#define IA32_PERFEVTSEL3              0x189U  // 393
#define IA32_PERFEVTSEL4              0x18AU  // 394
#define IA32_PERFEVTSEL5              0x18BU  // 395
#define IA32_PERFEVTSEL6              0x18CU  // 396
#define IA32_PERFEVTSEL7              0x18DU  // 397
#define IA32_PERFEVTSEL8              0x18EU  // 398
#define IA32_PERFEVTSEL9              0x18FU  // 399
// 18AH−194H, 394−404 N/A
#define IA32_OVERCLOCKING_STATUS      0x195U  // 405
// 196H−197H, 406−407 N/A
#define IA32_PERF_STATUS              0x198U  // 408
#define IA32_PERF_CTL                 0x199U  // 409
#define IA32_CLOCK_MODULATION         0x19AU  // 410
#define IA32_THERM_INTERRUPT          0x19BU  // 411
#define IA32_THERM_STATUS             0x19CU  // 412
#define IA32_MISC_ENABLE              0x1A0U  // 416
#define IA32_ENERGY_PERF_BIAS         0x1B0U  // 432
#define IA32_PACKAGE_THERM_STATUS     0x1B1U  // 433
#define IA32_PACKAGE_THERM_INTERRUPT  0x1B2U  // 434
#define IA32_XFD                      0x1C4U  // 452
#define IA32_XFD_ERR                  0x1C5U  // 453
#define IA32_DEBUGCTL                 0x1D9U  // 473 MSR_DEBUGCTLA, MSR_DEBUGCTLB
#define IA32_LER_FROM_IP              0x1DDU  // 477
#define IA32_LER_TO_IP                0x1DEU  // 478
#define IA32_LER_INFO                 0x1E0U  // 480
#define IA32_SMRR_PHYSBASE            0x1F2U  // 498
#define IA32_SMRR_PHYSMASK            0x1F3U  // 499
#define IA32_PLATFORM_DCA_CAP         0x1F8U  // 504
#define IA32_CPU_DCA_CAP              0x1F9U  // 505
#define IA32_DCA_0_CAP                0x1FAU  // 506

#define IA32_MTRR_PHYSBASE0    0x200U  // 512 MTRRphysBase0
#define IA32_MTRR_PHYSMASK0    0x201U  // 513
#define IA32_MTRR_PHYSBASE1    0x202U  // 514
#define IA32_MTRR_PHYSMASK1    0x203U  // 515
#define IA32_MTRR_PHYSBASE2    0x204U  // 516
#define IA32_MTRR_PHYSMASK2    0x205U  // 517
#define IA32_MTRR_PHYSBASE3    0x206U  // 518
#define IA32_MTRR_PHYSMASK3    0x207U  // 519
#define IA32_MTRR_PHYSBASE4    0x208U  // 520
#define IA32_MTRR_PHYSMASK4    0x209U  // 521
#define IA32_MTRR_PHYSBASE5    0x20AU  // 522
#define IA32_MTRR_PHYSMASK5    0x20BU  // 523
#define IA32_MTRR_PHYSBASE6    0x20CU  // 524
#define IA32_MTRR_PHYSMASK6    0x20DU  // 525
#define IA32_MTRR_PHYSBASE7    0x20EU  // 526
#define IA32_MTRR_PHYSMASK7    0x20FU  // 527
#define IA32_MTRR_PHYSBASE8    0x210U  // 528
#define IA32_MTRR_PHYSMASK8    0x211U  // 529
#define IA32_MTRR_PHYSBASE9    0x212U  // 530
#define IA32_MTRR_PHYSMASK9    0x213U  // 531
#define IA32_MTRR_FIX64K_00000 0x250U  // 592
#define IA32_MTRR_FIX16K_80000 0x258U  // 600
#define IA32_MTRR_FIX16K_A0000 0x259U  // 601
#define IA32_MTRR_FIX4K_C0000  0x268U  // 616 MTRRfix4K_C0000
#define IA32_MTRR_FIX4K_C8000  0x269U  // 617
#define IA32_MTRR_FIX4K_D0000  0x26AU  // 618
#define IA32_MTRR_FIX4K_D8000  0x26BU  // 619
#define IA32_MTRR_FIX4K_E0000  0x26CU  // 620
#define IA32_MTRR_FIX4K_E8000  0x26DU  // 621
#define IA32_MTRR_FIX4K_F0000  0x26EU  // 622
#define IA32_MTRR_FIX4K_F8000  0x26FU  // 623
#define IA32_PAT               0x277U  // 631

#define IA32_MCn_CTL2(n) (0x280U + n * 4)  // 640 + n * 4, 0 <= n < 32

#define IA32_MC0_CTL2  0x280U  // 640
#define IA32_MC1_CTL2  0x281U  // 641
#define IA32_MC2_CTL2  0x282U  // 642
#define IA32_MC3_CTL2  0x283U  // 643
#define IA32_MC4_CTL2  0x284U  // 644
#define IA32_MC5_CTL2  0x285U  // 645
#define IA32_MC6_CTL2  0x286U  // 646
#define IA32_MC7_CTL2  0x287U  // 647
#define IA32_MC8_CTL2  0x288U  // 648
#define IA32_MC9_CTL2  0x289U  // 649
#define IA32_MC10_CTL2 0x28AU  // 650
#define IA32_MC11_CTL2 0x28BU  // 651
#define IA32_MC12_CTL2 0x28CU  // 652
#define IA32_MC13_CTL2 0x28DU  // 653
#define IA32_MC14_CTL2 0x28EU  // 654
#define IA32_MC15_CTL2 0x28FU  // 655
#define IA32_MC16_CTL2 0x290U  // 656
#define IA32_MC17_CTL2 0x291U  // 657
#define IA32_MC18_CTL2 0x292U  // 658
#define IA32_MC19_CTL2 0x293U  // 659
#define IA32_MC20_CTL2 0x294U  // 660
#define IA32_MC21_CTL2 0x295U  // 661
#define IA32_MC22_CTL2 0x296U  // 662
#define IA32_MC23_CTL2 0x297U  // 663
#define IA32_MC24_CTL2 0x298U  // 664
#define IA32_MC25_CTL2 0x299U  // 665
#define IA32_MC26_CTL2 0x29AU  // 666
#define IA32_MC27_CTL2 0x29BU  // 667
#define IA32_MC28_CTL2 0x29CU  // 668
#define IA32_MC29_CTL2 0x29DU  // 669
#define IA32_MC30_CTL2 0x29EU  // 670
#define IA32_MC31_CTL2 0x29FU  // 671

#define IA32_INTEGRITY_STATUS 0x2DCU  // 732
#define IA32_MTRR_DEF_TYPE    0x2FFU  // 767

#define IA32_FIXED_CTR0               0x309U  // 777
#define IA32_FIXED_CTR1               0x30AU  // 778
#define IA32_FIXED_CTR2               0x30BU  // 779
#define IA32_FIXED_CTR3               0x30CU  // 780
#define IA32_FIXED_CTR4               0x30DU  // 781
#define IA32_FIXED_CTR5               0x30EU  // 782
#define IA32_FIXED_CTR6               0x30FU  // 783
#define IA32_PERF_CAPABILITIES        0x345U  // 837
#define IA32_FIXED_CTR_CTRL           0x38DU  // 909
#define IA32_PERF_GLOBAL_STATUS       0x38EU  // 910
#define IA32_PERF_GLOBAL_CTRL         0x38FU  // 911
#define IA32_PERF_GLOBAL_STATUS_RESET 0x390U  // 912
#define IA32_PERF_GLOBAL_STATUS_SET   0x391U  // 913
#define IA32_PERF_GLOBAL_INUSE        0x392U  // 914
#define IA32_PEBS_ENABLE              0x3F1U  // 1009

#define IA32_MCn_CTL(n)    (0x400U + n * 4)  // 1024 + n * 4, 0 <= n < 32
#define IA32_MCn_STATUS(n) (0x401U + n * 4)  // 1025 + n * 4, 0 <= n < 32
#define IA32_MCn_ADDR(n)   (0x402U + n * 4)  // 1026 + n * 4, 0 <= n < 32
#define IA32_MCn_MISC(n)   (0x403U + n * 4)  // 1027 + n * 4, 0 <= n < 32

#define IA32_MC0_CTL     0x400U  // 1024
#define IA32_MC0_STATUS  0x401U  // 1025
#define IA32_MC0_ADDR    0x402U  // 1026
#define IA32_MC0_MISC    0x403U  // 1027
#define IA32_MC1_CTL     0x404U  // 1028
#define IA32_MC1_STATUS  0x405U  // 1029
#define IA32_MC1_ADDR    0x406U  // 1030
#define IA32_MC1_MISC    0x407U  // 1031
#define IA32_MC2_CTL     0x408U  // 1032
#define IA32_MC2_STATUS  0x409U  // 1033
#define IA32_MC2_ADDR    0x40AU  // 1034
#define IA32_MC2_MISC    0x40BU  // 1035
#define IA32_MC3_CTL     0x40CU  // 1036
#define IA32_MC3_STATUS  0x40DU  // 1037
#define IA32_MC3_ADDR    0x40EU  // 1038
#define IA32_MC3_MISC    0x40FU  // 1039
#define IA32_MC4_CTL     0x410U  // 1040
#define IA32_MC4_STATUS  0x411U  // 1041
#define IA32_MC4_ADDR    0x412U  // 1042
#define IA32_MC4_MISC    0x413U  // 1043
#define IA32_MC5_CTL     0x414U  // 1044
#define IA32_MC5_STATUS  0x415U  // 1045
#define IA32_MC5_ADDR    0x416U  // 1046
#define IA32_MC5_MISC    0x417U  // 1047
#define IA32_MC6_CTL     0x418U  // 1048
#define IA32_MC6_STATUS  0x419U  // 1049
#define IA32_MC6_ADDR    0x41AU  // 1050
#define IA32_MC6_MISC    0x41BU  // 1051
#define IA32_MC7_CTL     0x41CU  // 1052
#define IA32_MC7_STATUS  0x41DU  // 1053
#define IA32_MC7_ADDR    0x41EU  // 1054
#define IA32_MC7_MISC    0x41FU  // 1055
#define IA32_MC8_CTL     0x420U  // 1056
#define IA32_MC8_STATUS  0x421U  // 1057
#define IA32_MC8_ADDR    0x422U  // 1058
#define IA32_MC8_MISC    0x423U  // 1059
#define IA32_MC9_CTL     0x424U  // 1060
#define IA32_MC9_STATUS  0x425U  // 1061
#define IA32_MC9_ADDR    0x426U  // 1062
#define IA32_MC9_MISC    0x427U  // 1063
#define IA32_MC10_CTL    0x428U  // 1064
#define IA32_MC10_STATUS 0x429U  // 1065
#define IA32_MC10_ADDR   0x42AU  // 1066
#define IA32_MC10_MISC   0x42BU  // 1067
#define IA32_MC11_CTL    0x42CU  // 1068
#define IA32_MC11_STATUS 0x42DU  // 1069
#define IA32_MC11_ADDR   0x42EU  // 1070
#define IA32_MC11_MISC   0x42FU  // 1071
#define IA32_MC12_CTL    0x430U  // 1072
#define IA32_MC12_STATUS 0x431U  // 1073
#define IA32_MC12_ADDR   0x432U  // 1074
#define IA32_MC12_MISC   0x433U  // 1075
#define IA32_MC13_CTL    0x434U  // 1076
#define IA32_MC13_STATUS 0x435U  // 1077
#define IA32_MC13_ADDR   0x436U  // 1078
#define IA32_MC13_MISC   0x437U  // 1079
#define IA32_MC14_CTL    0x438U  // 1080
#define IA32_MC14_STATUS 0x439U  // 1081
#define IA32_MC14_ADDR   0x43AU  // 1082
#define IA32_MC14_MISC   0x43BU  // 1083
#define IA32_MC15_CTL    0x43CU  // 1084
#define IA32_MC15_STATUS 0x43DU  // 1085
#define IA32_MC15_ADDR   0x43EU  // 1086
#define IA32_MC15_MISC   0x43FU  // 1087
#define IA32_MC16_CTL    0x440U  // 1088
#define IA32_MC16_STATUS 0x441U  // 1089
#define IA32_MC16_ADDR   0x442U  // 1090
#define IA32_MC16_MISC   0x443U  // 1091
#define IA32_MC17_CTL    0x444U  // 1092
#define IA32_MC17_STATUS 0x445U  // 1093
#define IA32_MC17_ADDR   0x446U  // 1094
#define IA32_MC17_MISC   0x447U  // 1095
#define IA32_MC18_CTL    0x448U  // 1096
#define IA32_MC18_STATUS 0x449U  // 1097
#define IA32_MC18_ADDR   0x44AU  // 1098
#define IA32_MC18_MISC   0x44BU  // 1099
#define IA32_MC19_CTL    0x44CU  // 1100
#define IA32_MC19_STATUS 0x44DU  // 1101
#define IA32_MC19_ADDR   0x44EU  // 1102
#define IA32_MC19_MISC   0x44FU  // 1103
#define IA32_MC20_CTL    0x450U  // 1104
#define IA32_MC20_STATUS 0x451U  // 1105
#define IA32_MC20_ADDR   0x452U  // 1106
#define IA32_MC20_MISC   0x453U  // 1107
#define IA32_MC21_CTL    0x454U  // 1108
#define IA32_MC21_STATUS 0x455U  // 1109
#define IA32_MC21_ADDR   0x456U  // 1110
#define IA32_MC21_MISC   0x457U  // 1111
#define IA32_MC22_CTL    0x458U  // 1112
#define IA32_MC22_STATUS 0x459U  // 1113
#define IA32_MC22_ADDR   0x45AU  // 1114
#define IA32_MC22_MISC   0x45BU  // 1115
#define IA32_MC23_CTL    0x45CU  // 1116
#define IA32_MC23_STATUS 0x45DU  // 1117
#define IA32_MC23_ADDR   0x45EU  // 1118
#define IA32_MC23_MISC   0x45FU  // 1119
#define IA32_MC24_CTL    0x460U  // 1120
#define IA32_MC24_STATUS 0x461U  // 1121
#define IA32_MC24_ADDR   0x462U  // 1122
#define IA32_MC24_MISC   0x463U  // 1123
#define IA32_MC25_CTL    0x464U  // 1124
#define IA32_MC25_STATUS 0x465U  // 1125
#define IA32_MC25_ADDR   0x466U  // 1126
#define IA32_MC25_MISC   0x467U  // 1127
#define IA32_MC26_CTL    0x468U  // 1128
#define IA32_MC26_STATUS 0x469U  // 1129
#define IA32_MC26_ADDR   0x46AU  // 1130
#define IA32_MC26_MISC   0x46BU  // 1131
#define IA32_MC27_CTL    0x46CU  // 1132
#define IA32_MC27_STATUS 0x46DU  // 1133
#define IA32_MC27_ADDR   0x46EU  // 1134
#define IA32_MC27_MISC   0x46FU  // 1135
#define IA32_MC28_CTL    0x470U  // 1136
#define IA32_MC28_STATUS 0x471U  // 1137
#define IA32_MC28_ADDR   0x472U  // 1138
#define IA32_MC28_MISC   0x473U  // 1139
#define IA32_MC29_CTL    0x474U  // 1140
#define IA32_MC29_STATUS 0x475U  // 1141
#define IA32_MC29_ADDR   0x476U  // 1142
#define IA32_MC29_MISC   0x477U  // 1143
#define IA32_MC30_CTL    0x478U  // 1144
#define IA32_MC30_STATUS 0x479U  // 1145
#define IA32_MC30_ADDR   0x47AU  // 1146
#define IA32_MC30_MISC   0x47BU  // 1147
#define IA32_MC31_CTL    0x47CU  // 1148
#define IA32_MC31_STATUS 0x47DU  // 1149
#define IA32_MC31_ADDR   0x47EU  // 1150
#define IA32_MC31_MISC   0x47FU  // 1151

#define IA32_VMX_BASIC               0x480U  // 1152
#define IA32_VMX_PINBASED_CTLS       0x481U  // 1153
#define IA32_VMX_PROCBASED_CTLS      0x482U  // 1154
#define IA32_VMX_EXIT_CTLS           0x483U  // 1155
#define IA32_VMX_ENTRY_CTLS          0x484U  // 1156
#define IA32_VMX_MISC                0x485U  // 1157
#define IA32_VMX_CR0_FIXED0          0x486U  // 1158
#define IA32_VMX_CR0_FIXED1          0x487U  // 1159
#define IA32_VMX_CR4_FIXED0          0x488U  // 1160
#define IA32_VMX_CR4_FIXED1          0x489U  // 1161
#define IA32_VMX_VMCS_ENUM           0x48AU  // 1162
#define IA32_VMX_PROCBASED_CTLS2     0x48BU  // 1163
#define IA32_VMX_EPT_VPID_CAP        0x48CU  // 1164
#define IA32_VMX_TRUE_PINBASED_CTLS  0x48DU  // 1165
#define IA32_VMX_TRUE_PROCBASED_CTLS 0x48EU  // 1166
#define IA32_VMX_TRUE_EXIT_CTLS      0x48FU  // 1167
#define IA32_VMX_TRUE_ENTRY_CTLS     0x490U  // 1168
#define IA32_VMX_VMFUNC              0x491U  // 1169
#define IA32_VMX_PROCBASED_CTLS3     0x492U  // 1170
#define IA32_VMX_EXIT_CTLS2          0x493U  // 1171

#define IA32_A_PMC0 0x4C1U  // 1217
#define IA32_A_PMC1 0x4C2U  // 1218
#define IA32_A_PMC2 0x4C3U  // 1219
#define IA32_A_PMC3 0x4C4U  // 1220
#define IA32_A_PMC4 0x4C5U  // 1221
#define IA32_A_PMC5 0x4C6U  // 1222
#define IA32_A_PMC6 0x4C7U  // 1223
#define IA32_A_PMC7 0x4C8U  // 1224
#define IA32_A_PMC8 0x4C9U  // 1225
#define IA32_A_PMC9 0x4CAU  // 1226

#define IA32_MCG_EXT_CTL    0x4D0U  // 1232
#define IA32_SGX_SVN_STATUS 0x500U  // 1280

#define IA32_RTIT_OUTPUT_BASE      0x560U  // 1376
#define IA32_RTIT_OUTPUT_MASK_PTRS 0x561U  // 1377
#define IA32_RTIT_CTL              0x570U  // 1392
#define IA32_RTIT_STATUS           0x571U  // 1393
#define IA32_RTIT_CR3_MATCH        0x572U  // 1394
#define IA32_RTIT_ADDR0_A          0x580U  // 1408
#define IA32_RTIT_ADDR0_B          0x581U  // 1409
#define IA32_RTIT_ADDR1_A          0x582U  // 1410
#define IA32_RTIT_ADDR1_B          0x583U  // 1411
#define IA32_RTIT_ADDR2_A          0x584U  // 1412
#define IA32_RTIT_ADDR2_B          0x585U  // 1413
#define IA32_RTIT_ADDR3_A          0x586U  // 1414
#define IA32_RTIT_ADDR3_B          0x587U  // 1415

#define IA32_DS_AREA                  0x600U  // 1536
#define IA32_U_CET                    0x6A0U  // 1696
#define IA32_S_CET                    0x6A2U  // 1698
#define IA32_PL0_SSP                  0x6A4U  // 1700
#define IA32_PL1_SSP                  0x6A5U  // 1701
#define IA32_PL2_SSP                  0x6A6U  // 1702
#define IA32_PL3_SSP                  0x6A7U  // 1703
#define IA32_INTERRUPT_SSP_TABLE_ADDR 0x6A8U  // 1704
#define IA32_TSC_DEADLINE             0x6E0U  // 1760
#define IA32_PKRS                     0x6E1U  // 1761
#define IA32_PM_ENABLE                0x770U  // 1904
#define IA32_HWP_CAPABILITIES         0x771U  // 1905
#define IA32_HWP_REQUEST_PKG          0x772U  // 1906
#define IA32_HWP_REQUEST              0x774U  // 1908
#define IA32_PECI_HWP_REQUEST_INFO    0x775U  // 1909
#define IA32_HWP_CTL                  0x776U  // 1910
#define IA32_HWP_STATUS               0x777U  // 1911
#define IA32_MCU_EXT_SERVICE          0x7A3U  // 1955
#define IA32_MCU_ROLLBACK_MIN_ID      0x7A4U  // 1956
#define IA32_MCU_STAGING_MBOX_ADDR    0x7A5U  // 1957

#define IA32_ROLLBACK_SIGN_ID_0  0x7B0U  // 1968
#define IA32_ROLLBACK_SIGN_ID_1  0x7B1U  // 1969
#define IA32_ROLLBACK_SIGN_ID_2  0x7B2U  // 1970
#define IA32_ROLLBACK_SIGN_ID_3  0x7B3U  // 1971
#define IA32_ROLLBACK_SIGN_ID_4  0x7B4U  // 1972
#define IA32_ROLLBACK_SIGN_ID_5  0x7B5U  // 1973
#define IA32_ROLLBACK_SIGN_ID_6  0x7B6U  // 1974
#define IA32_ROLLBACK_SIGN_ID_7  0x7B7U  // 1975
#define IA32_ROLLBACK_SIGN_ID_8  0x7B8U  // 1976
#define IA32_ROLLBACK_SIGN_ID_9  0x7B9U  // 1977
#define IA32_ROLLBACK_SIGN_ID_10 0x7BAU  // 1978
#define IA32_ROLLBACK_SIGN_ID_11 0x7BBU  // 1979
#define IA32_ROLLBACK_SIGN_ID_12 0x7BCU  // 1980
#define IA32_ROLLBACK_SIGN_ID_13 0x7BDU  // 1981
#define IA32_ROLLBACK_SIGN_ID_14 0x7BEU  // 1982
#define IA32_ROLLBACK_SIGN_ID_15 0x7BFU  // 1983

#define IA32_X2APIC_APICID      0x802U  // 2050
#define IA32_X2APIC_VERSION     0x803U  // 2051
#define IA32_X2APIC_TPR         0x808U  // 2056
#define IA32_X2APIC_PPR         0x80AU  // 2058
#define IA32_X2APIC_EOI         0x80BU  // 2059
#define IA32_X2APIC_LDR         0x80DU  // 2061
#define IA32_X2APIC_SIVR        0x80FU  // 2063
#define IA32_X2APIC_ISR0        0x810U  // 2064
#define IA32_X2APIC_ISR1        0x811U  // 2065
#define IA32_X2APIC_ISR2        0x812U  // 2066
#define IA32_X2APIC_ISR3        0x813U  // 2067
#define IA32_X2APIC_ISR4        0x814U  // 2068
#define IA32_X2APIC_ISR5        0x815U  // 2069
#define IA32_X2APIC_ISR6        0x816U  // 2070
#define IA32_X2APIC_ISR7        0x817U  // 2071
#define IA32_X2APIC_TMR0        0x818U  // 2072
#define IA32_X2APIC_TMR1        0x819U  // 2073
#define IA32_X2APIC_TMR2        0x81AU  // 2074
#define IA32_X2APIC_TMR3        0x81BU  // 2075
#define IA32_X2APIC_TMR4        0x81CU  // 2076
#define IA32_X2APIC_TMR5        0x81DU  // 2077
#define IA32_X2APIC_TMR6        0x81EU  // 2078
#define IA32_X2APIC_TMR7        0x81FU  // 2079
#define IA32_X2APIC_IRR0        0x820U  // 2080
#define IA32_X2APIC_IRR1        0x821U  // 2081
#define IA32_X2APIC_IRR2        0x822U  // 2082
#define IA32_X2APIC_IRR3        0x823U  // 2083
#define IA32_X2APIC_IRR4        0x824U  // 2084
#define IA32_X2APIC_IRR5        0x825U  // 2085
#define IA32_X2APIC_IRR6        0x826U  // 2086
#define IA32_X2APIC_IRR7        0x827U  // 2087
#define IA32_X2APIC_ESR         0x828U  // 2088
#define IA32_X2APIC_LVT_CMCI    0x82FU  // 2095
#define IA32_X2APIC_ICR         0x830U  // 2096
#define IA32_X2APIC_LVT_TIMER   0x832U  // 2098
#define IA32_X2APIC_LVT_THERMAL 0x833U  // 2099
#define IA32_X2APIC_LVT_PMI     0x834U  // 2100
#define IA32_X2APIC_LVT_LINT0   0x835U  // 2101
#define IA32_X2APIC_LVT_LINT1   0x836U  // 2102
#define IA32_X2APIC_LVT_ERROR   0x837U  // 2103
#define IA32_X2APIC_INIT_COUNT  0x838U  // 2104
#define IA32_X2APIC_CUR_COUNT   0x839U  // 2105
#define IA32_X2APIC_DIV_CONF    0x83EU  // 2110
#define IA32_X2APIC_SELF_IPI    0x83FU  // 2111

#define IA32_TME_CAPABILITY   0x981U  // 2433
#define IA32_TME_ACTIVATE     0x982U  // 2434
#define IA32_TME_EXCLUDE_MASK 0x983U  // 2435
#define IA32_TME_EXCLUDE_BASE 0x984U  // 2436

#define IA32_UINTR_RR          0x985U  // 2437
#define IA32_UINTR_HANDLER     0x986U  // 2438
#define IA32_UINTR_STACKADJUST 0x987U  // 2439
#define IA32_UINTR_MISC        0x988U  // 2440
#define IA32_UINTR_PD          0x989U  // 2441
#define IA32_UINTR_TT          0x98AU  // 2442

#define IA32_COPY_STATUS4        0x990U  // 2448
#define IA32_IWKEYBACKUP_STATUS5 0x991U  // 2449
#define IA32_TME_CLEAR_SAVED_KEY 0x9FBU  // 2555
#define IA32_DEBUG_INTERFACE     0xC80U  // 3200

#define IA32_L3_QOS_CFG                0xC81U  // 3201
#define IA32_L2_QOS_CFG                0xC82U  // 3202
#define IA32_L3_IO_QOS_CFG             0xC83U  // 3203
#define IA32_RESOURCE_PRIORITY         0xC88U  // 3208
#define IA32_RESOURCE_PRIORITY_PKG     0xC89U  // 3209
#define IA32_QM_EVTSEL                 0xC8DU  // 3213
#define IA32_QM_CTR                    0xC8EU  // 3214
#define IA32_PQR_ASSOC                 0xC8FU  // 3215
// C90H−D8FH, 3216−3471 Reserved MSR Address Space for CAT Mask Registers
#define IA32_L3_MASK_0                 0xC90U        // 3216
#define IA32_L3_MASK_n(n)              (0xC90U + n)  // 0xC90+n, 3216+n
// D10H−D4FH, 3344−3407 Reserved MSR Address Space for L2 CAT Mask Registers
#define IA32_L2_MASK_0                 0xD10U        // 3344
#define IA32_L2_MASK_n(n)              (0xD10U + n)  // D10H+n, 3344+n
#define IA32_L2_MASK_8                 0xD18U        // 3352
#define IA32_L2_MASK_9                 0xD19U        // 3353
#define IA32_L2_MASK_10                0xD1AU        // 3354
#define IA32_L2_MASK_11                0xD1BU        // 3355
#define IA32_L2_MASK_12                0xD1CU        // 3356
#define IA32_L2_MASK_13                0xD1DU        // 3357
#define IA32_L2_MASK_14                0xD1EU        // 3358
#define IA32_L2_MASK_15                0xD1FU        // 3359
#define IA32_L2_QOS_EXT_BW_THRTL_0     0xD50U        // 3408
#define IA32_L2_QOS_EXT_BW_THRTL_1     0xD51U        // 3409
#define IA32_L2_QOS_EXT_BW_THRTL_2     0xD52U        // 3410
#define IA32_L2_QOS_EXT_BW_THRTL_3     0xD53U        // 3411
#define IA32_L2_QOS_EXT_BW_THRTL_4     0xD54U        // 3412
#define IA32_L2_QOS_EXT_BW_THRTL_5     0xD55U        // 3413
#define IA32_L2_QOS_EXT_BW_THRTL_6     0xD56U        // 3414
#define IA32_L2_QOS_EXT_BW_THRTL_7     0xD57U        // 3415
#define IA32_L2_QOS_EXT_BW_THRTL_8     0xD58U        // 3416
#define IA32_L2_QOS_EXT_BW_THRTL_9     0xD59U        // 3417
#define IA32_L2_QOS_EXT_BW_THRTL_10    0xD5AU        // 3418
#define IA32_L2_QOS_EXT_BW_THRTL_11    0xD5BU        // 3419
#define IA32_L2_QOS_EXT_BW_THRTL_12    0xD5CU        // 3420
#define IA32_L2_QOS_EXT_BW_THRTL_13    0xD5DU        // 3421
#define IA32_L2_QOS_EXT_BW_THRTL_14    0xD5EU        // 3422
#define IA32_BNDCFGS                   0xD90U        // 3472
#define IA32_COPY_LOCAL_TO_PLATFORM    0xD91U        // 3473
#define IA32_COPY_PLATFORM_TO_LOCAL    0xD92U        // 3474
#define IA32_PASID                     0xD93U        // 3475
#define IA32_XSS                       0xDA0U        // 3488
#define IA32_PKG_HDC_CTL               0xDB0U        // 3504
#define IA32_PM_CTL1                   0xDB1U        // 3505
#define IA32_THREAD_STALL              0xDB2U        // 3506
#define IA32_QOS_CORE_BW_THRTL_0       0xE00U        // 3584
#define IA32_QOS_CORE_BW_THRTL_1       0xE01U        // 3585
#define IA32_LBR_x_INFO(x)             (0x1200 + x)  // 1200H−121FH, 4608−4639
#define IA32_SEAMRR_BASE               0x1400U       // 5120
#define IA32_SEAMRR_MASK               0x1401U       // 5121
#define IA32_MCU_CONTROL               0x1406U       // 5126
#define IA32_LBR_CTL                   0x14CEU       // 5326
#define IA32_LBR_DEPTH                 0x14CFU       // 5327
#define IA32_LBR_x_FROM_IP(x)          (0x1500 + x)  // 1500H−151FH, 5376−5407
#define IA32_LBR_x_TO_IP(x)            (0x1600 + x)  // 1600H−161FH, 5632−5663
#define IA32_HW_FEEDBACK_PTR           0x17D0U       // 6096
#define IA32_HW_FEEDBACK_CONFIG        0x17D1U       // 6097
#define IA32_THREAD_FEEDBACK_CHAR      0x17D2U       // 6098
#define IA32_HW_FEEDBACK_THREAD_CONFIG 0x17D4U       // 6100
#define IA32_HRESET_ENABLE             0x17DAU       // 6106
#define IA32_PMC_GP0_CTR               0x1900U       // 6400
#define IA32_PMC_GP0_CFG_A             0x1901U       // 6401
#define IA32_PMC_GP0_CFG_C             0x1903U       // 6403
#define IA32_PMC_GP1_CTR               0x1904U       // 6404
#define IA32_PMC_GP1_CFG_A             0x1905U       // 6405
#define IA32_PMC_GP1_CFG_C             0x1907U       // 6407
#define IA32_PMC_GP2_CTR               0x1908U       // 6408
#define IA32_PMC_GP2_CFG_A             0x1909U       // 6409
#define IA32_PMC_GP2_CFG_B             0x190AU       // 6410
#define IA32_PMC_GP2_CFG_C             0x190BU       // 6411
#define IA32_PMC_GP3_CTR               0x190CU       // 6412
#define IA32_PMC_GP3_CFG_A             0x190DU       // 6413
#define IA32_PMC_GP3_CFG_B             0x190EU       // 6414
#define IA32_PMC_GP3_CFG_C             0x190FU       // 6415
#define IA32_PMC_GP4_CTR               0x1910U       // 6416
#define IA32_PMC_GP4_CFG_A             0x1911U       // 6417
#define IA32_PMC_GP4_CFG_B             0x1912U       // 6418
#define IA32_PMC_GP4_CFG_C             0x1913U       // 6419
#define IA32_PMC_GP5_CTR               0x1914U       // 6420
#define IA32_PMC_GP5_CFG_A             0x1915U       // 6421
#define IA32_PMC_GP5_CFG_B             0x1916U       // 6422
#define IA32_PMC_GP5_CFG_C             0x1917U       // 6423
#define IA32_PMC_GP6_CTR               0x1918U       // 6424
#define IA32_PMC_GP6_CFG_A             0x1919U       // 6425
#define IA32_PMC_GP6_CFG_B             0x191AU       // 6426
#define IA32_PMC_GP6_CFG_C             0x191BU       // 6427
#define IA32_PMC_GP7_CTR               0x191CU       // 6428
#define IA32_PMC_GP7_CFG_A             0x191DU       // 6429
#define IA32_PMC_GP7_CFG_B             0x191EU       // 6430
#define IA32_PMC_GP7_CFG_C             0x191FU       // 6431
#define IA32_PMC_GP8_CTR               0x1920U       // 6432
#define IA32_PMC_GP8_CFG_A             0x1921U       // 6433
#define IA32_PMC_GP9_CTR               0x1924U       // 6436
#define IA32_PMC_GP9_CFG_A             0x1925U       // 6437
#define IA32_PMC_FX0_CTR               0x1980U       // 6528
#define IA32_PMC_FX0_CFG_B             0x1982U       // 6530
#define IA32_PMC_FX0_CFG_C             0x1983U       // 6531
#define IA32_PMC_FX1_CTR               0x1984U       // 6532
#define IA32_PMC_FX1_CFG_B             0x1986U       // 6534
#define IA32_PMC_FX1_CFG_C             0x1987U       // 6532
#define IA32_PMC_FX2_CTR               0x1988U       // 6536
#define IA32_PMC_FX2_CFG_C             0x198BU       // 6539
#define IA32_PMC_FX3_CTR               0x198CU       // 6540
#define IA32_PMC_FX4_CTR               0x1990U       // 6544
#define IA32_PMC_FX4_CFG_C             0x1993U       // 6547
#define IA32_PMC_FX5_CTR               0x1994U       // 6548
#define IA32_PMC_FX5_CFG_C             0x1997U       // 6551
#define IA32_PMC_FX6_CTR               0x1998U       // 6552
#define IA32_PMC_FX6_CFG_C             0x199BU       // 6555
#define IA32_UARCH_MISC_CTL            0x1B01U       // 6913
// 4000_0000H−4000_00FFH Reserved MSR Address Space
#define IA32_EFER                      0xC0000080U
#define IA32_STAR                      0xC0000081U
#define IA32_LSTAR                     0xC0000082U
#define IA32_CSTAR                     0xC0000083U
#define IA32_FMASK                     0xC0000084U
#define IA32_FS_BASE                   0xC0000100U
#define IA32_GS_BASE                   0xC0000101U
#define IA32_KERNEL_GS_BASE            0xC0000102U
#define IA32_TSC_AUX                   0xC0000103U