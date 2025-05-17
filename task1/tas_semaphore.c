#include "tas_semaphore.h"
#include <sched.h>
#include <stdatomic.h>

void spinlock_init(atomic_flag *lock);
void spinlock_acquire(atomic_flag *lock);
void spinlock_release(atomic_flag *lock);


void semaphore_init(semaphore* sem, int initial_value) {
    atomic_init(&sem->available_resources, initial_value);
    sem->max_resources = initial_value;
    spinlock_init(&sem->spinlock); // Initialize the spinlock to be unlocked
}

//Implements semaphore_wait using the TAS spinlock mechanism.

void semaphore_wait(semaphore* sem) {
    while (1) {
        spinlock_acquire(&sem->spinlock);
        
        if (sem->available_resources > 0) {
            atomic_fetch_sub(&sem->available_resources, 1);
            spinlock_release(&sem->spinlock);
            return;
        }

        spinlock_release(&sem->spinlock);
        sched_yield();
    }  
}

//Implements semaphore_signal using the TAS spinlock mechanism.
 
void semaphore_signal(semaphore* sem) {
    spinlock_acquire(&sem->spinlock);
    if (sem->available_resources < sem->max_resources) {
            atomic_fetch_add(&sem->available_resources, 1);
    }
    spinlock_release(&sem->spinlock);
}


// Spinlock functions
void spinlock_init(atomic_flag *lock) {
    atomic_flag_clear(lock);
}

void spinlock_acquire(atomic_flag *lock) {
    while (atomic_flag_test_and_set(lock)) {
        // Busy-wait until the lock is released
        sched_yield(); // Yield the processor to allow other threads to run
    }
}

void spinlock_release(atomic_flag *lock) {
    atomic_flag_clear(lock);
}