#pragma once

#include "clib/bitmap.h"
#include "mem/phymem.h"

#include <stdint.h>

void *early_mem_alloc_pages(uint16_t n);
void  early_mem_free_pages(void *addr, uint16_t n);
