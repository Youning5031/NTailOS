#pragma once

#define PIC_MST_EVE_PORT 0x20
#define PIC_MST_ODD_PORT 0x21
#define PIC_SLV_EVE_PORT 0xA0
#define PIC_SLV_ODD_PORT 0xA1

bool apic_init(void);