#include <inttypes.h>
#include "local_storage.h"

/*
 * TODO: Define the global TLS array.
 */
tls_data_t g_tls[MAX_THREADS];

/*
 * TODO: Implement init_storage to initialize g_tls.
 */
void init_storage(void) {
    // TODO: Set all thread_id fields to -1 and data pointers to NULL.
    for (int i = 0; i < MAX_THREADS; i++) {
        g_tls[i].thread_id = -1;
        g_tls[i].data = NULL;
    }
}

/*
 * TODO: Implement tls_thread_alloc to allocate a TLS entry for the calling thread.
 */
void tls_thread_alloc(void) {
    // TODO: Use your synchronization mechanism to safely allocate an entry.
    int first_free_index = -1;
    int64_t thread_id = pthread_self(); // Get the thread ID

    // Check if the thread ID already exists or find first free entry ow
    for (int i = 0; i < MAX_THREADS; i++) {
        if (g_tls[i].thread_id == thread_id) {
            // Thread ID already exists, return without allocating
            return;
        }
        if (g_tls[i].thread_id == -1) {
            first_free_index = i; 
        }
    }
    
    if (first_free_index == -1) {
        // No free entry available
        printf("thread %" PRId64 " failed to initialize, not enough space", thread_id);
        exit(1);
    }
    // Initialize the new entry
    g_tls[first_free_index].thread_id = thread_id;
      
}

/*
 * TODO: Implement get_tls_data to retrieve the TLS data for the calling thread.
 */
void* get_tls_data(void) {
    // TODO: Search for the calling thread's entry and return its data.

    // If the thread ID is not found, print an error message
    printf("thread  %" PRId64 " hasn’t been initialized in the TLS", thread_id);
    exit(2);
    return NULL;
}

/*
 * TODO: Implement set_tls_data to update the TLS data for the calling thread.
 */
void set_tls_data(void* data) {
    // TODO: Search for the calling thread's entry and set its data.
    int64_t thread_id = pthread_self(); // Get the thread ID

    for (int i = 0; i < MAX_THREADS; i++) {
        if (g_tls[i].thread_id == thread_id) {
            g_tls[i].data = data; // Set the data for the calling thread
            return;
        }
    }
    // If the thread ID is not found, print an error message
    printf("thread  %" PRId64 " hasn’t been initialized in the TLS", thread_id);
    exit(2);
}

/*
 * TODO: Implement tls_thread_free to free the TLS entry for the calling thread.
 */
void tls_thread_free(void) {
    // TODO: Reset the thread_id and data in the corresponding TLS entry.
}