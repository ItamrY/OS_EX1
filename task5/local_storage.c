#include <inttypes.h>
#include "local_storage.h"
#include <stdio.h> 
#include <stdlib.h>
#include <stdatomic.h>
#include <sched.h>

/*
 Define the global TLS array.
 */
tls_data_t g_tls[MAX_THREADS];

static atomic_bool tls_lock = 0;

//tls_lock - spinlock implementation to allow for safe mapping of threads
void tls_lock_acquire() {
    while(atomic_exchange(&tls_lock,1)) {
        sched_yield();
    }
}

void tls_lock_release() {
    atomic_store(&tls_lock, 0);
}

// iterate through entire array, set all thread_id to -1 and all data to NULL
void init_storage(void) {
    for (int i = 0; i < MAX_THREADS; i++) {
        g_tls[i].thread_id = -1;
        g_tls[i].data = NULL;
    }
    atomic_store(&tls_lock,0); //after loop - make sure array is locked
}

// allocate a TLS entry for the calling thread.
void tls_thread_alloc(void) {
    int first_free_index = -1;
    int64_t thread_id = (int64_t) pthread_self(); // Get the thread ID
    tls_lock_acquire(); //open spinlock

    // Check if the thread ID already exists or find first free entry ow
    for (int i = 0; i < MAX_THREADS; i++) {
        if (g_tls[i].thread_id == thread_id) {
            // Thread ID already exists, return without allocating
            tls_lock_release();
            return;
        }
        // if thread_id doesn't exist AND i "finds" an empty spot - first_free_index will be locationi in array
        if (first_free_index == -1 && g_tls[i].thread_id == -1) {
            first_free_index = i; 
        }
    }
    
    if (first_free_index == -1) {
        // No free entry available
        //printf("thread %" PRId64 " failed to initialize, not enough space", thread_id);
        fprintf(stderr, "thread %" PRId64 " failed to initialize, not enough space", thread_id);
        tls_lock_release();
        exit(1);
    }
    // Initialize the new entry
    g_tls[first_free_index].thread_id = thread_id;
    //g_tls[first_free_index].data = NULL;
    tls_lock_release(); //make sure lock is closed   
}

//Implement get_tls_data to retrieve the TLS data for the calling thread.
 
void* get_tls_data(void) {
    int64_t thread_id = (int64_t) pthread_self(); // Get the thread ID
    tls_lock_acquire();

    for(int i=0; i < MAX_THREADS; i++){
        if(g_tls[i].thread_id == thread_id) {
            void* data = g_tls[i].data; //take the pointer data of the matching thread_id
            tls_lock_release();
            return data;
        }
    }

    // If the thread ID is not found, print an error message
    fprintf(stderr, "thread  %" PRId64 " hasn’t been initialized in the TLS", thread_id);
    tls_lock_release();
    exit(2);
}

//Implement set_tls_data to update the TLS data for the calling thread.
 
void set_tls_data(void* data) {
    int64_t thread_id = (int64_t) pthread_self(); // Get the thread ID
    tls_lock_acquire();

    for (int i = 0; i < MAX_THREADS; i++) {
        if (g_tls[i].thread_id == thread_id) {
            g_tls[i].data = data; // Set the data for the calling thread
            tls_lock_release();
            return;
        }
    }
    // If the thread ID is not found, print an error message
    fprintf(stderr, "thread  %" PRId64 " hasn’t been initialized in the TLS", thread_id);
    //printf("thread  %" PRId64 " hasn’t been initialized in the TLS", thread_id);
    tls_lock_release();
    exit(2);
}

//Implement tls_thread_free to free the TLS entry for the calling thread.

void tls_thread_free(void) {
    int64_t thread_id = (int64_t) pthread_self(); // Get the thread ID
    tls_lock_acquire();

    for (int i=0; i < MAX_THREADS; i++) {
        if(g_tls[i].thread_id == thread_id) {
            g_tls[i].thread_id = -1; // reset
            g_tls[i].data = NULL; // reset
            tls_lock_release();

             printf("thread  %" PRId64 " has been deleted from the TLS\n", thread_id);
            return;
        }
    }
    tls_lock_release(); //if nothing found
}
