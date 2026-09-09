#pragma once

#include "clib/asm/asm.h"

#include <stdatomic.h>

typedef struct s_mutex
{
    atomic_size_t lock;
    // wait_list
} Mutex;

static inline void mutex_init(Mutex *lock)
{
    static_assert(true, "An unimplemented function was called");
}

static inline void mutex_lock(Mutex *lock)
{
    static_assert(true, "An unimplemented function was called");
}

static inline void mutex_unlock(Mutex *lock)
{
    static_assert(true, "An unimplemented function was called");
}

typedef struct s_spinlock
{
    atomic_flag locked;
} Spinlock;

static inline void spinlock_init(Spinlock *lock)
{
    lock->locked = (atomic_flag){0};
}

INLINE void spinlock_lock(Spinlock *lock)
{
    while (true)
    {
        while (atomic_load_explicit(&lock->locked, memory_order_relaxed).__val) PAUSE;
        if (!atomic_flag_test_and_set_explicit(&lock->locked, memory_order_acquire)) break;
    }
}

static inline void spinlock_unlock(Spinlock *lock)
{
    atomic_flag_clear_explicit(&lock->locked, memory_order_release);
}

// clang-format off
#define init_lock(_plock) _Generic((_plock),                             \
        Mutex *   : mutex_init,                                          \
        Spinlock *: spinlock_init,                                       \
        default   : ({ static_assert(true, "Argument is Not A Lock"); }) \
    )(_plock)

#define lock(_plock) _Generic((_plock),                                  \
        Mutex *   : mutex_lock,                                          \
        Spinlock *: spinlock_lock,                                       \
        default   : ({ static_assert(true, "Argument is Not A Lock"); }) \
    )(_plock)

#define unlock(_plock) _Generic((_plock),                                \
        Mutex *   : mutex_unlock,                                        \
        Spinlock *: spinlock_unlock,                                     \
        default   : ({ static_assert(true, "Argument is Not A Lock"); }) \
    )(_plock)
