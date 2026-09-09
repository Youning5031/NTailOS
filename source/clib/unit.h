#pragma once

#if !defined(ASM_FILE) && !defined(LD_FILE)

#  ifdef NEED_MEM_UINTS
#    define B(x)  (x)
#    define KB(x) (x * 0x400LL)
#    define MB(x) (x * 0x100000LL)
#    define GB(x) (x * 0x40000000LL)
#    define TB(x) (x * 0x10000000000LL)
#  endif

#else

#  ifdef NEED_MEM_UINTS
#    define B(x)  (x)
#    define KB(x) (x * 0x400)
#    define MB(x) (x * 0x100000)
#    define GB(x) (x * 0x40000000)
#    define TB(x) (x * 0x10000000000)
#  endif

#endif