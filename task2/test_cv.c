#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "cond_var.h"

#define THREADS 4

condition_variable cv;
ticket_lock lock;
int ready = 0;

void* thread_fn(void* arg) {
    ticketlock_acquire(&lock);
    while (!ready) {
        condition_variable_wait(&cv, &lock);
    }
    printf("Thread %ld resumed\n", (long)arg);
    ticketlock_release(&lock);
    return NULL;
}

int main() {
    pthread_t threads[THREADS];
    condition_variable_init(&cv);
    ticketlock_init(&lock);

    for (long i = 0; i < THREADS; ++i) {
        pthread_create(&threads[i], NULL, thread_fn, (void*)i);
    }

    sleep(1);  // Let threads go to sleep
    ticketlock_acquire(&lock);
    ready = 1;
    condition_variable_broadcast(&cv);  // Wake all threads
    ticketlock_release(&lock);

    for (int i = 0; i < THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    printf("✅ Condition variable broadcast test completed.\n");
    return 0;
}
