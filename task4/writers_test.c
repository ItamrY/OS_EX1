#include "rw_lock.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdatomic.h>

atomic_int shared = 0;
rwlock lock;

void* writer(void* arg) {
    rwlock_acquire_write(&lock);
    printf("Writer in\n");
    shared = 42; // write something
    sleep(1);    // simulate work
    printf("Writer out\n");
    rwlock_release_write(&lock);
    return NULL;
}

void* reader(void* arg) {
    rwlock_acquire_read(&lock);
    printf("Reader in (sees shared=%d)\n", shared);
    sleep(1);  // simulate work
    printf("Reader out\n");
    rwlock_release_read(&lock);
    return NULL;
}

int main() {
    rwlock_init(&lock);

    pthread_t t1, t2, t3;

    pthread_create(&t1, NULL, reader, NULL);
    sleep(1); // let reader start

    pthread_create(&t2, NULL, writer, NULL);
    sleep(1); // let writer wait

    pthread_create(&t3, NULL, reader, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    return 0;
}