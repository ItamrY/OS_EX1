#ifndef RW_LOCK_H
#define RW_LOCK_H

#include <stdatomic.h>
#include "../task2/ticket_lock.h"
#include "../task3/cond_var.h"

/*
 * A fair read-write lock that allows multiple readers or one writer.
 * Writers are given priority to prevent starvation.
 */
typedef struct {
    ticket_lock lock;               // internal mutual exclusion lock
    condition_variable readers_ok;  // condition variable for waiting readers
    condition_variable writers_ok;  // condition variable for waiting writers

    atomic_int active_readers;      // number of readers holding the lock
    atomic_int active_writers;      // 1 if a writer holds the lock, 0 otherwise
    atomic_int waiting_writers;     // number of writers waiting
} rwlock;

/*
 * Initializes the read-write lock.
 */
void rwlock_init(rwlock* lock);

/*
 * Acquires the lock for reading.
 */
void rwlock_acquire_read(rwlock* lock);

/*
 * Releases the lock after reading.
 */
void rwlock_release_read(rwlock* lock);

/*
 * Acquires the lock for writing. Ensures exclusive access.
 */
void rwlock_acquire_write(rwlock* lock);

/*
 * Releases the lock after writing.
 */
void rwlock_release_write(rwlock* lock);

#endif // RW_LOCK_H
