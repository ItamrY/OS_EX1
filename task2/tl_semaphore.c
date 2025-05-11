// #include "tl_semaphore.h"
// #include <stdio.h>

// void semaphore_init(semaphore* sem, int initial_value) {
//     sem->available_resources = initial_value;
//     ticketlock_init(&sem->tl); // Initialize the spinlock to be unlocked
// }


// void semaphore_wait(semaphore* sem) {
//     int my_ticket = atomic_fetch_add(&sem->tl.ticket, 1); // get ticket

//     while (1) {
//         while (atomic_load(&sem->tl.cur_ticket) != my_ticket) {
//             sched_yield();
//         }

//         if (sem->available_resources > 0) {
//             sem->available_resources--;
//             printf("Thread got ticket #%d and acquired semaphore\n", my_ticket);
//             ticketlock_release(&sem->tl);
//             return;
//         }

//         sched_yield();
//     }
// }

// void semaphore_signal(semaphore* sem) {
//     ticketlock_release(&sem->tl);
//     sem->available_resources++;
//     ticketlock_release(&sem->tl);
// }

#include "tl_semaphore.h"
#include <stdio.h>

void semaphore_init(semaphore* sem, int initial_value) {
    sem->available_resources = initial_value;
    ticketlock_init(&sem->tl);
}

void semaphore_wait(semaphore* sem) {
    while (1) {
        int my_ticket = ticketlock_acquire(&sem->tl);  // this blocks until it's our turn

        if (sem->available_resources > 0) {
            sem->available_resources--;
            printf("Thread got ticket #%d and acquired semaphore\n", my_ticket);
            ticketlock_release(&sem->tl);
            return;
        }

        ticketlock_release(&sem->tl);  // not our turn, give it to next thread
        sched_yield();                 // let others run
    }
}

void semaphore_signal(semaphore* sem) {
    ticketlock_acquire(&sem->tl);
    sem->available_resources++;
    ticketlock_release(&sem->tl);
}

