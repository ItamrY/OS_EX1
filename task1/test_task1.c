#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "tas_semaphore.h"

semaphore sem;
int done = 0;

void* waiting_thread(void* arg) {
    semaphore_wait(&sem);
    printf("Thread passed the semaphore\n");
    done = 1;
    return NULL;
}

int main() {
    pthread_t t;
    semaphore_init(&sem, 0);

    pthread_create(&t, NULL, waiting_thread, NULL);
    sleep(1); // Give thread time to block

    if (done == 0) {
        printf("Thread is correctly waiting...\n");
    } else {
        printf("Error: thread should not have passed yet!\n");
    }

    semaphore_signal(&sem); // Unblock the thread
    pthread_join(t, NULL);

    return 0;
}
