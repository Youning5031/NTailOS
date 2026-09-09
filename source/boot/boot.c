#include "boot.h"

#include "multiboot2.h"

#include <stdarg.h>
#include <stdint.h>

BOOT_CODE uint32_t get_boot_info(BootTag *boot_tag, uint32_t except_info_count, ...)
{
    va_list args;
    va_start(args);
    uint32_t  except_info_types[except_info_count];
    BootTag **except_info[except_info_count];
    for (uint32_t i = 0; i < except_info_count; i++)
    {
        except_info_types[i] = va_arg(args, uint32_t);
        except_info[i] = va_arg(args, BootTag **);
    }
    va_end(args);

    uint32_t found_info = 0;
    while (boot_tag->type != MULTIBOOT_TAG_TYPE_END)
    {
        for (uint32_t i = 0; i < except_info_count; i++)
        {
            if (*(except_info[i]) != nullptr) continue;
            if (boot_tag->type == except_info_types[i])
            {
                *(except_info[i]) = boot_tag;
                found_info++;
                break;
            }
        }
        boot_tag = (BootTag *)(((uintptr_t)boot_tag + boot_tag->size + 7) & ~(uintptr_t)7);
    }

    return found_info;
}

#include "clib/print.h"

void print_boot_info(BootInfo *boot_info)
{
    kprintln();
    kprintln("Boot Info Size: %u", boot_info->total_size);
    for (BootTag *tag = boot_info->tags; tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (BootTag *)(((uintptr_t)tag + tag->size + 7) & ~(uintptr_t)7))
    {
        kprintln("Tag Type: %u", tag->type);
        kprintln("Tag Size: %u", tag->size);
        kprintln("Tag Addr: 0x%lx", (uintptr_t)tag);
        switch (tag->type)
        {
        case MULTIBOOT_TAG_TYPE_CMDLINE:
            kprintln("Command Line: %s", ((struct multiboot_tag_string *)tag)->string);
            break;

        case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
            kprintln("Bootloader Name: %s", ((struct multiboot_tag_string *)tag)->string);
            break;

        case MULTIBOOT_TAG_TYPE_MODULE: {
            auto module_tag = (struct multiboot_tag_module *)tag;
            kprintln("Module Address Start: 0x%lx", module_tag->mod_start);
            kprintln("Module Address End:   0x%lx", module_tag->mod_end);
            kprintln("Module String: 0x%lx", module_tag->cmdline);
            break;
        }
        case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO: {
            auto basic_meminfo_tag = (struct multiboot_tag_basic_meminfo *)tag;
            kprintln("Lower Memory Size: %u", basic_meminfo_tag->mem_lower);
            kprintln("\tLower Memory: 0x%lx ~ 0x%lx", 0ULL, (uintptr_t)(basic_meminfo_tag->mem_lower) << 10);
            kprintln("Upper Memory Size: %u", basic_meminfo_tag->mem_upper);
            kprintln(
                "\tUpper Memory: 0x%lx ~ 0x%lx", 1ULL << 20, (uintptr_t)((basic_meminfo_tag->mem_upper) + 1024) << 10);
            break;
        }
        case MULTIBOOT_TAG_TYPE_BOOTDEV: {
            auto bootdev_tag = (struct multiboot_tag_bootdev *)tag;
            kprintln("Bios Device: 0x%x", bootdev_tag->biosdev);
            kprintln("Partition: 0x%x", bootdev_tag->slice);
            kprintln("Subpartition: 0x%x", bootdev_tag->part);
            break;
        }
        case MULTIBOOT_TAG_TYPE_MMAP: {
            auto     mmap_tag = (struct multiboot_tag_mmap *)tag;
            uint32_t count = (mmap_tag->size - sizeof(struct multiboot_tag)) / mmap_tag->entry_size;
            kprintln("MMAP Entries Count: %u", count);
            kprintln("MMAP Entry Version: 0x%x", mmap_tag->entry_version);
            for (uint32_t i = 0; i < count; i++)
            {
                kprintln("Memory Region %u", i);
                kprintln(
                    "\taddr: 0x%lx ~ 0x%lx", mmap_tag->entries[i].addr,
                    mmap_tag->entries[i].addr + mmap_tag->entries[i].len);
                kprintln("\tlen: %lu", mmap_tag->entries[i].len);
                const char *type_str;
                switch (mmap_tag->entries[i].type)
                {
                case MULTIBOOT_MEMORY_AVAILABLE: type_str = "AVAILABLE"; break;
                case MULTIBOOT_MEMORY_RESERVED: type_str = "RESERVED"; break;
                case MULTIBOOT_MEMORY_ACPI_RECLAIMABLE: type_str = "ACPI RECLAIMABLE"; break;
                case MULTIBOOT_MEMORY_NVS: type_str = "NVS"; break;
                case MULTIBOOT_MEMORY_BADRAM: type_str = "BADRAM"; break;
                default: type_str = "*UNKNOWN*"; break;
                }
                kprintln("\ttype: %u - %s", mmap_tag->entries[i].type, type_str);
            }
            break;
        }
        case MULTIBOOT_TAG_TYPE_VBE: {
            auto vbe_tag = (struct multiboot_tag_vbe *)tag;
            kprintln("VBE Mode: %u", vbe_tag->vbe_mode);
            kprintln("VBE Interface Segment: %u", vbe_tag->vbe_interface_seg);
            kprintln("VBE Interface Offset: %u", vbe_tag->vbe_interface_off);
            kprintln("VBE Interface Length: %u", vbe_tag->vbe_interface_len);
            kprintln("VBE Control Info Address: 0x%lx", vbe_tag->vbe_control_info.external_specification);
            kprintln("VBE Mode Info Mode Address: 0x%lx", vbe_tag->vbe_mode_info.external_specification);
            break;
        }
        case MULTIBOOT_TAG_TYPE_FRAMEBUFFER: {
            auto framebuffer_tag = (struct multiboot_tag_framebuffer *)tag;
            kprintln("Framebuffer Address: 0x%lx", framebuffer_tag->common.framebuffer_addr);
            kprintln("Framebuffer Pitch: %u", framebuffer_tag->common.framebuffer_pitch);
            kprintln("Framebuffer Width: %u", framebuffer_tag->common.framebuffer_width);
            kprintln("Framebuffer Height: %u", framebuffer_tag->common.framebuffer_height);
            kprintln("Framebuffer BPP: %u", framebuffer_tag->common.framebuffer_bpp);
            switch (framebuffer_tag->common.framebuffer_type)
            {
            case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED:
                kprintln("type: %u - %s", framebuffer_tag->common.framebuffer_type, "INDEX");
                kprintln("Framebuffer Colors Count: %u", framebuffer_tag->framebuffer_palette_num_colors);
                for (uint16_t i = 0; i < framebuffer_tag->framebuffer_palette_num_colors; i++)
                {
                    kprintln(
                        "\tColor %u: #%x", i,
                        framebuffer_tag->framebuffer_palette[i].red << 16
                            | framebuffer_tag->framebuffer_palette[i].green << 8
                            | framebuffer_tag->framebuffer_palette[i].blue);
                }
                break;
            case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:
                kprintln("type: %u - %s", framebuffer_tag->common.framebuffer_type, "RGB");
                kprintln("Red:");
                kprintln("\tField Position: %u", framebuffer_tag->framebuffer_red_field_position);
                kprintln("\tMask Size: %u", framebuffer_tag->framebuffer_red_mask_size);
                kprintln("Green:");
                kprintln("\tField Position: %u", framebuffer_tag->framebuffer_green_field_position);
                kprintln("\tMask Size: %u", framebuffer_tag->framebuffer_green_mask_size);
                kprintln("Blue:");
                kprintln("\tField Position: %u", framebuffer_tag->framebuffer_blue_field_position);
                kprintln("\tMask Size: %u", framebuffer_tag->framebuffer_blue_mask_size);
                break;
            case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:
                kprintln("type: %u - %s", framebuffer_tag->common.framebuffer_type, "EGA TEXT");
                break;
            default: kprintln("type: %u - %s", framebuffer_tag->common.framebuffer_type, "*UNKNOWN*"); break;
            }
            break;
        }
        case MULTIBOOT_TAG_TYPE_ELF_SECTIONS: {
            auto elf_sections_tag = (struct multiboot_tag_elf_sections *)tag;
            kprintln("ELF Sections Count: %u", elf_sections_tag->num);
            kprintln("ELF Sections Size: %u", elf_sections_tag->entsize);
            kprintln("ELF Sections Shndx: %u", elf_sections_tag->shndx);
            kprintln("ELF Sections: %s", elf_sections_tag->sections);
            break;
        }
        case MULTIBOOT_TAG_TYPE_APM: {
            auto apm_tag = (struct multiboot_tag_apm *)tag;
            kprintln("APM Version: %u.%u", apm_tag->version >> 8, apm_tag->version & 0xFF);
            kprintln("Protected Mode 32-bit Code Segment: 0x%x", apm_tag->cseg);
            kprintln("Entry Point Offset: 0x%x", apm_tag->offset);
            kprintln("Protected Mode 16-bit Code Segment: 0x%x", apm_tag->cseg_16);
            kprintln("Protected Mode 16-bit Data Segment: 0x%x", apm_tag->dseg);
            kprintln("Flags: 0x%x", apm_tag->flags);
            kprintln("cseg_len: 0x%x", apm_tag->cseg_len);
            kprintln("cseg_16_len: 0x%x", apm_tag->cseg_16_len);
            kprintln("dseg_len: 0x%x", apm_tag->dseg_len);
            break;
        }

        case MULTIBOOT_TAG_TYPE_EFI32:
            kprintln("EFI32 System Table Pointer: 0x%x", ((struct multiboot_tag_efi32 *)tag)->pointer);
            break;

        case MULTIBOOT_TAG_TYPE_EFI64:
            kprintln("EFI64 System Table Pointer: 0x%lx", ((struct multiboot_tag_efi64 *)tag)->pointer);
            break;

        case MULTIBOOT_TAG_TYPE_SMBIOS: {
            auto smbios_tag = (struct multiboot_tag_smbios *)tag;
            kprintln("SMBIOS Version: %u.%u", smbios_tag->major, smbios_tag->minor);
            // 计算表格长度：总大小减去固定部分（type + size + major + minor + reserved[6] = 16 字节）
            uint32_t tables_len = tag->size - 16;
            kprintln("SMBIOS Tables Address: 0x%lx, Length: %u", (uintptr_t)smbios_tag->tables, tables_len);
            break;
        }
        case MULTIBOOT_TAG_TYPE_ACPI_OLD: {
            auto     acpi_old_tag = (struct multiboot_tag_old_acpi *)tag;
            uint32_t rsdp_len = tag->size - 8;  // 减去 type 和 size
            kprintln("ACPI Old RSDP Address: 0x%lx, Length: %u", (uintptr_t)acpi_old_tag->rsdp, rsdp_len);
            break;
        }
        case MULTIBOOT_TAG_TYPE_ACPI_NEW: {
            auto     acpi_new_tag = (struct multiboot_tag_new_acpi *)tag;
            uint32_t rsdp_len = tag->size - 8;
            kprintln("ACPI New RSDP Address: 0x%lx, Length: %u", (uintptr_t)acpi_new_tag->rsdp, rsdp_len);
            break;
        }
        case MULTIBOOT_TAG_TYPE_NETWORK: {
            auto     net_tag = (struct multiboot_tag_network *)tag;
            uint32_t dhcp_len = tag->size - 8;
            kprintln("Network DHCP ACK Address: 0x%lx, Length: %u", (uintptr_t)net_tag->dhcpack, dhcp_len);
            break;
        }
        case MULTIBOOT_TAG_TYPE_EFI_MMAP: {
            auto efi_mmap_tag = (struct multiboot_tag_efi_mmap *)tag;
            kprintln("EFI Memory Map Descriptor Size: %u", efi_mmap_tag->descr_size);
            kprintln("EFI Memory Map Descriptor Version: %u", efi_mmap_tag->descr_vers);
            uint32_t mmap_len = tag->size - 16;  // 减去固定部分 16 字节
            uint32_t num_entries = mmap_len / efi_mmap_tag->descr_size;
            kprintln("EFI Memory Map Entries Count: %u", num_entries);
            kprintln("EFI Memory Map Address: 0x%lx", (uintptr_t)efi_mmap_tag->efi_mmap);
            break;
        }
        case MULTIBOOT_TAG_TYPE_EFI_BS: {
            kprintln("EFI Boot Services Not Terminated");
            break;
        }
        case MULTIBOOT_TAG_TYPE_EFI32_IH:
            kprintln("EFI32 Image Handle: 0x%x", ((struct multiboot_tag_efi32_ih *)tag)->pointer);
            break;

        case MULTIBOOT_TAG_TYPE_EFI64_IH:
            kprintln("EFI64 Image Handle: 0x%lx", ((struct multiboot_tag_efi64_ih *)tag)->pointer);
            break;

        case MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR:
            kprintln("Load Base Address: 0x%x", ((struct multiboot_tag_load_base_addr *)tag)->load_base_addr);
            break;

        default:
            kprintln("Unknown Tag, exit.");
            return;
            break;
        }
    }
    kprintln("Boot Info End.");
    return;
}
