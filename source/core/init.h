#pragma once

#define INIT_PRI_MAX     255
#define INIT_PRI_DEFAULT 127
#define INIT_PRI_MIN     101

#define INIT_PRI_cpu_feature INIT_PRI_MAX
#define INIT_PRI_serial      (INIT_PRI_MAX - 1)
#define INIT_PRI_interrupt   (INIT_PRI_MAX - 2)
#define INIT_PRI_segment     (INIT_PRI_MAX - 3)
#define INIT_PRI_early_mem   INIT_PRI_DEFAULT
#define INIT_PRI_sobj        (INIT_PRI_DEFAULT - 1)
#define INIT_PRI_memory      (INIT_PRI_DEFAULT - 2)

// #define INIT_PRI_simd        INIT_PRI_DEFAULT
// #define INIT_PRI_vga         INIT_PRI_DEFAULT

typedef struct multiboot_tag BootTag;

typedef bool (*Constructor)(BootTag *);
#define INIT(init_name) CONSTRUCTOR(INIT_PRI_##init_name) bool init_##init_name(BootTag *boot_tags)
