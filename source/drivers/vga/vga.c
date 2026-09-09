#include "vga.h"

#include "boot/boot.h"
#include "clib/alloca.h"
#include "clib/print.h"
#include "core/init.h"

INIT(vga)
{
    kprintln("\nInitializing VGA");
    struct multiboot_tag_framebuffer *framebuffer;
    get_boot_info(boot_tags, 1, MULTIBOOT_TAG_TYPE_FRAMEBUFFER, &framebuffer);
    return true;
}