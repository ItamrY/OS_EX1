#include "rw_lock.h"
#include <sched.h>

void rwlock_init(rwlock* lock) {
    ticketlock_init(&lock->lock);
    condition_variable_init(&lock->readers_ok);
    condition_variable_init(&lock->writers_ok);
    atomic_init(&lock->active_readers, 0);
    atomic_init(&lock->active_writers, 0);
    atomic_init(&lock->waiting_writers, 0);
}


void rwlock_acquire_read(rwlock* lock) {
    ticketlock_acquire(&lock->lock);

    // Wait while a writer is active or waiting (to prevnet writer starvation)
    while (atomic_load(&lock->active_writers) > 0 || atomic_load(&lock->waiting_writers) > 0) {
        condition_variable_wait(&lock->readers_ok, &lock->lock);
    }

    atomic_fetch_add(&lock->active_readers, 1);

    ticketlock_release(&lock->lock);
}


void rwlock_release_read(rwlock* lock) {
    ticketlock_acquire(&lock->lock);

    atomic_fetch_sub(&lock->active_readers, 1);

    // If this was the last reader and writers are waiting, signal one
    if (atomic_load(&lock->active_readers) == 0 && atomic_load(&lock->waiting_writers) > 0) {
        condition_variable_signal(&lock->writers_ok);
    }

    ticketlock_release(&lock->lock);
}


void rwlock_acquire_write(rwlock* lock) {
    ticketlock_acquire(&lock->lock);

    atomic_fetch_add(&lock->waiting_writers, 1);

    // Wait while any readers or another writer is active
    //while (atomic_load(&lock->active_readers) > 0 || atomic_load(&lock->active_writers) > 0) {
    while (atomic_load(&lock->active_writers) > 0) {

        condition_variable_wait(&lock->writers_ok, &lock->lock);
    }

    atomic_fetch_sub(&lock->waiting_writers, 1);
    atomic_store(&lock->active_writers, 1);

    ticketlock_release(&lock->lock);
}


void rwlock_release_write(rwlock* lock) {
    ticketlock_acquire(&lock->lock);

    atomic_store(&lock->active_writers, 0);

    // Prefer writers over readers to prevent writer starvation
    if (atomic_load(&lock->waiting_writers) > 0) {
        condition_variable_signal(&lock->writers_ok);
    } else {
        condition_variable_broadcast(&lock->readers_ok);
    }

    ticketlock_release(&lock->lock);
}
