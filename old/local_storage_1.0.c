#include "rw_lock.h"
#include <sched.h>
#include "../task3/cond_var.h"


void rwlock_init(rwlock* lock) {
    atomic_init(&lock->readers_amount, 0);
    atomic_init(&lock->writer_active, 0);
    atomic_init(&lock->writer_waiting, 0);

}


void rwlock_acquire_read(rwlock* lock) {
    while (1) {
        // Wait while a writer is active or waiting
        while (atomic_load(&lock->writer_active) || atomic_load(&lock->writer_waiting)) {
            sched_yield();
        }

        // increment reader amount
        atomic_fetch_add(&lock->readers_amount, 1);

        ///////check
        // Double-check that a writer didn’t slip in
        if (!atomic_load(&lock->writer_active) && !atomic_load(&lock->writer_waiting)) {
            break; // Safe to read
        }

        // Undo and try again
        atomic_fetch_sub(&lock->readers_amount, 1);
        sched_yield();
    }
}


void rwlock_release_read(rwlock* lock) {
    atomic_fetch_sub(&lock->readers_amount, 1);
}


void rwlock_acquire_write(rwlock* lock) {
    // Signal writer is waiting
    atomic_store(&lock->writer_waiting, 1);

    // Wait for all readers to exit
    while (atomic_load(&lock->readers_amount) > 0) {
        sched_yield();
    }

    // Begin writing
    atomic_store(&lock->writer_active, 1);
    atomic_store(&lock->writer_waiting, 0);

}


void rwlock_release_write(rwlock* lock) {
    atomic_store(&lock->writer_active, 0);
}
