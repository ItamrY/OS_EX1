#include "tl_semaphore.h"
#include <stdio.h>
#include <sched.h>

void semaphore_init(semaphore* sem, int initial_value) {
    atomic_init(&sem->available_resources, initial_value);
    sem->max_resources = initial_value;

    atomic_init(&sem->ticket, 0);
    atomic_init(&sem->cur_ticket, 0);
}

// COMPARE WITH SOMEONE
void semaphore_wait(semaphore* sem) {
    int my_ticket = atomic_fetch_add(&sem->ticket, 1);

    while (atomic_load(&sem->cur_ticket) < my_ticket || atomic_load(&sem->available_resources) == 0) {
        sched_yield();
    }
    
    atomic_fetch_sub(&sem->available_resources, 1);
    atomic_fetch_add(&sem->cur_ticket, 1);
    // printf("Thread got ticket #%d and acquired semaphore\n", my_ticket);              
}

void semaphore_signal(semaphore* sem) {
    if (sem->available_resources < sem->max_resources) {
            atomic_fetch_add(&sem->available_resources, 1);
    }
}



