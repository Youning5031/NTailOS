#pragma once

#include <stdint.h>

#define BUILDIO(type)                                   \
    INLINE void __out_##type(uint16_t port, type value) \
    {                                                   \
        asm("out %1, %0" : : "a"(value), "Nd"(port));   \
    }                                                   \
                                                        \
    INLINE type __in_##type(uint16_t port)              \
    {                                                   \
        type value;                                     \
        asm("in %0, %1" : "=a"(value) : "Nd"(port));    \
        return value;                                   \
    }

BUILDIO(uint8_t)
BUILDIO(uint16_t)
BUILDIO(uint32_t)

#define out(w, port, value) __out_uint##w##_t((uint16_t)(port), (uint##w##_t)(value))

#define in(w, port) __in_uint##w##_t((uint16_t)(port))

#undef BUILDIO