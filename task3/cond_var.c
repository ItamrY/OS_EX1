#include "cond_var.h"
#include <sched.h>
#include <stdbool.h>

// Ticket lock function declarations
void ticketlock_init(ticket_lock* lock);
int ticketlock_acquire(ticket_lock* lock);
void ticketlock_release(ticket_lock* lock);


void condition_variable_init(condition_variable* cv) {
    // TODO: Initialize internal fields.
    atomic_init(&cv->waiting_count, 0);
    atomic_init(&cv->signal_count, 0);
}


void condition_variable_wait(condition_variable* cv, ticket_lock* ext_lock) {
    // TODO: Increase waiter count, release ext_lock, wait until signaled, then reacquire ext_lock.
    atomic_fetch_add(&cv->waiting_count, 1);
    ticketlock_release(ext_lock);

    while (atomic_load_explicit(&cv->signal_count, memory_order_acquire) == 0) {
        sched_yield();
    }

    ticketlock_acquire(ext_lock);

    // Decrease "used" signal count and waiting count
    atomic_fetch_sub(&cv->signal_count, 1);
    atomic_fetch_sub(&cv->waiting_count, 1); 
}


void condition_variable_signal(condition_variable* cv) {
    // TODO: Signal one waiting thread.
    if (atomic_load(&cv->waiting_count) > 0) {
        atomic_fetch_add_explicit(&cv->signal_count, 1, memory_order_release);
    }
}


void condition_variable_broadcast(condition_variable* cv) {
    int waiting_count = atomic_load(&cv->waiting_count);
    atomic_fetch_add(&cv->signal_count, waiting_count);
}


// Ticket lock functions
void ticketlock_init(ticket_lock* lock) {
    atomic_init(&lock->ticket, 0);
    atomic_init(&lock->cur_ticket, 0);
}

int ticketlock_acquire(ticket_lock* lock) {
    // get my ticket
    int my_ticket = atomic_fetch_add(&lock->ticket, 1);

    // wait until it is my turn
    while (atomic_load(&lock->cur_ticket) != my_ticket) {
        sched_yield();
    }
    return my_ticket;
}

void ticketlock_release(ticket_lock* lock) {
    atomic_fetch_add(&lock->cur_ticket, 1);
}
