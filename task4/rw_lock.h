#ifndef RW_LOCK_H
#define RW_LOCK_H

#include <stdatomic.h>

/*
 * A fair read-write lock that allows multiple readers or one writer.
 * Writers are given priority to prevent starvation.
 */
typedef struct {
    atomic_bool writer_active;      //true if a writer is currently writing
    atomic_bool writer_waiting;     //treu if writer is waiting to go in
    atomic_int readers_amount;      // amount of readers at any given time
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
