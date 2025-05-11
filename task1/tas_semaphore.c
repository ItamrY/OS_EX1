#include "tas_semaphore.h"
#include <sched.h>
#include <stdatomic.h>



void semaphore_init(semaphore* sem, int initial_value) {
    sem->available_resources = initial_value;
    // atomic_flag_clear(&sem->lock);
    spinlock_init(&sem->lock); // Initialize the spinlock to be unlocked
}

/*
 * TODO: Implement semaphore_wait using the TAS spinlock mechanism.
 */
void semaphore_wait(semaphore* sem) {
    while (1) {
        spinlock_acquire(&sem->lock);
        
        if (sem->available_resources > 0) {
            sem->available_resources--;
            spinlock_release(&sem->lock);
            return;
        }

        spinlock_release(&sem->lock);
        sched_yield();
    }
    
    
}

/*
 * TODO: Implement semaphore_signal using the TAS spinlock mechanism.
 */
void semaphore_signal(semaphore* sem) {
    spinlock_acquire(&sem->lock);
    sem->available_resources++;
    spinlock_release(&sem->lock);
}