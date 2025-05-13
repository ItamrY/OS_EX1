#include "local_storage.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 3  // Try changing to 101 to test overflow

void* thread_func(void* arg) {
    int id = *(int*)arg;

    printf("🧵 Thread %d: Allocating TLS...\n", id);
    tls_thread_alloc();

    int* my_data = malloc(sizeof(int));
    *my_data = id * 100;

    printf("🧵 Thread %d: Setting TLS data to %d\n", id, *my_data);
    set_tls_data(my_data);

    int* read_back = get_tls_data();
    printf("🧵 Thread %d: Retrieved TLS data = %d\n", id, *read_back);

    tls_thread_free();
    printf("🧵 Thread %d: Freed TLS\n", id);

    free(my_data);
    return NULL;
}

int main() {
    printf("🚀 Initializing TLS system...\n");
    init_storage();

    printf("🔄 Launching %d threads to test normal TLS usage...\n", NUM_THREADS);
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i + 1;
        if (pthread_create(&threads[i], NULL, thread_func, &ids[i]) != 0) {
            perror("pthread_create");
            return 1;
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\n✅ All threads completed TLS lifecycle successfully.\n");

    // Edge Case 1: Get without allocating (should fail with exit(2))
    printf("\n❗️ Testing get_tls_data() without allocation (should exit with code 2)...\n");
    // Uncomment below line to test this edge case
    // get_tls_data();

    // Edge Case 2: Exceeding MAX_THREADS
    printf("\n❗️ Testing tls_thread_alloc() overflow (more than MAX_THREADS)...\n");

    for (int i = 0; i < MAX_THREADS + 1; i++) {
        int* id = malloc(sizeof(int));
        *id = i + 1000;  // just to differentiate
        pthread_t t;
        if (pthread_create(&t, NULL, thread_func, id) != 0) {
            perror("pthread_create");
        }
        pthread_detach(t);  // Avoid join; let threads clean up
    }

    sleep(2); // Allow detached threads to finish
    printf("\n🧪 Test finished (if thread 101 tried to alloc, it should have exited with code 1).\n");

    return 0;
}
