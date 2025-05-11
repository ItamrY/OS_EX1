#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include "tl_semaphore.h"

#define NUM_THREADS 10
#define SEM_INIT_VALUE 3

semaphore sem;

void* thread_func(void* arg) {
    int id = *(int*)arg;
    free(arg); // free dynamically allocated id

    printf("Thread %d: waiting for semaphore\n", id);
    semaphore_wait(&sem);

    printf("Thread %d: acquired semaphore\n", id);
    sleep(1); // simulate critical section

    printf("Thread %d: releasing semaphore\n", id);
    semaphore_signal(&sem);

    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];

    semaphore_init(&sem, SEM_INIT_VALUE);

    for (int i = 0; i < NUM_THREADS; ++i) {
        int* id = malloc(sizeof(int));
        *id = i;
        pthread_create(&threads[i], NULL, thread_func, id);
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    printf("All threads finished.\n");
    return 0;
}
