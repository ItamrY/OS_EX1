#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "cond_var.h"

#define NUM_THREADS 4

void ticketlock_init(ticket_lock* lock);
int ticketlock_acquire(ticket_lock* lock);
void ticketlock_release(ticket_lock* lock);

ticket_lock mutex;
condition_variable cond;
int shared_state = 0;

void* worker(void* arg) {
    int id = *(int*)arg;
    free(arg);

    ticketlock_acquire(&mutex);

    while (shared_state == 0) {
        printf("Thread %d: waiting\n", id);
        condition_variable_wait(&cond, &mutex);
    }

    printf("Thread %d: proceeding (shared_state = %d)\n", id, shared_state);
    ticketlock_release(&mutex);

    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];

    ticketlock_init(&mutex);
    condition_variable_init(&cond);

    // Create waiting threads
    for (int i = 0; i < NUM_THREADS; ++i) {
        int* id = malloc(sizeof(int));
        *id = i;
        pthread_create(&threads[i], NULL, worker, id);
    }

    sleep(2); // Give threads time to start and wait

    // Signal one thread
    ticketlock_acquire(&mutex);
    shared_state = 1;
    printf("Main: signaling one thread\n");
    condition_variable_signal(&cond);
    ticketlock_release(&mutex);

    sleep(2); // Let one thread proceed

    // Broadcast to remaining threads
    ticketlock_acquire(&mutex);
    printf("Main: broadcasting to remaining threads\n");
    condition_variable_broadcast(&cond);
    ticketlock_release(&mutex);

    // Join all threads
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    printf("Main: test complete\n");
    return 0;
}
