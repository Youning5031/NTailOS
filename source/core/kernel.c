#include "boot/boot.h"
#include "clib/asm/asm.h"
#include "clib/asm/farop.h"
#include "clib/assert.h"
#include "clib/print.h"
#include "cpu/cpu.h"
#include "init.h"
#include "mem/mem_early.h"
#include "mem/memory.h"
#include "mem/phymem.h"
#include "segment.h"

#include <cpuid.h>
#include <stddef.h>
#include <stdint.h>

void init(BootTag *boot_tags);

void kernel_main(uint32_t magic, BootInfo *boot_info)
{
    init(boot_info->tags);

    print_cpu_feature();

    kprintln("addr = 0x%lx", test_gs_64bit_offset());

    HLT;
}

void init(BootTag *boot_tags)
{
    extern const Constructor _ctors_start[], _ctors_end[];
    for (const Constructor *constructor = far_arr_ptr(_ctors_start); constructor < far_arr_ptr(_ctors_end);
         constructor++)
    {
        assert((*constructor)(boot_tags), "Init failed: constructor = 0x%lx", *constructor);
    }
    return;
}