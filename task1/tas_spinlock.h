#ifndef TAS_SPINLOCK_H
#define TAS_SPINLOCK_H

#include <stdatomic.h>


// Type definition for the test-and-set spinlock
typedef struct {
    atomic_flag lock; // Atomic flag for the lock
} spinlock_t;

// Function prototypes
void spinlock_init(spinlock_t *spinlock);

void spinlock_acquire(spinlock_t *spinlock);

void spinlock_release(spinlock_t *spinlock);

#endif // TAS_SPINLOCK_H