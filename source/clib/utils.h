#pragma once

#if !defined(ASM_FILE) && !defined(LD_FILE)

// set u64 range [from, to)
#  define SET_BITS_U64(from, to) (((1ULL << (to - from)) - 1) << from)
// set u32 range [from, to)
#  define SET_BITS_U32(from, to) (((1U << (to - from)) - 1) << from)

#else  // ASM_FILE LD_FILE

// set u64 range [from, to)
#  define SET_BITS(from, to) (((1 << (to - from)) - 1) << from)

#endif  // ASM_FILE LD_FILE