#ifndef TICKET_LOCK_H
#define TICKET_LOCK_H

#include <stdatomic.h>
#include <sched.h>


typedef struct {
    atomic_int ticket;
    atomic_int cur_ticket;
} ticket_lock;

void ticketlock_init(ticket_lock* lock);

int ticketlock_acquire(ticket_lock* lock);

void ticketlock_release(ticket_lock* lock);

#endif // TICKET_LOCK_H