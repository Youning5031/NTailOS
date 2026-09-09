#pragma once

#include <stddef.h>

#define containerof(ptr, type, member)                    \
    ({                                                    \
        const auto _mptr = ptr;                           \
        (type *)((char *)_mptr - offsetof(type, member)); \
    })
