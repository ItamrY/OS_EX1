#include "cp_pattern.h"
#include <stdio.h>
#include <stdlib.h>

// Added includes
#include "cond_var.h"
#include "queue.h"

#include <inttypes.h>
#include <stdatomic.h>
#include <pthread.h>
#include <stdbool.h>


//static int passed_numbers[MAX_SIZE] = {0}; // Array to keep track of finished numbers
static atomic_bool passed_numbers[MAX_SIZE];
static int consumer_array_size = 0; // Number of consumer threads
static int producer_array_size = 0; // Number of producer threads

static pthread_t* producer_threads; // Pointer to producers array
static pthread_t* consumer_threads; // Pointer to consumers array
static atomic_int produced_count = 0; // Counts how many numbers were produced
static atomic_int consumed_count = 0; // Counts how many numbers were consumed

static condition_variable cv; // Condition variable for synchronization
static ticket_lock queue_lock; // External lock for synchronization
static queue number_queue; // Passes the produced number to the consumers


// The "Main" function of the producer
void* producer_main(void* arg) {
    (void)arg;
    // Endless loop so each thread keeps producing until all numbers were produced.
    while(1) {
        int64_t id = (int64_t) pthread_self();
        int number = rand() % MAX_SIZE; // Generate a random number from 0 to 999,999 
        
        // Check if finished producing
        if (atomic_load(&produced_count) >= MAX_SIZE) {
            return NULL;
        }
        
        // Acquire lock
        ticketlock_acquire(&queue_lock); 
        // if (passed_numbers[number] == 1) {
        //     ticketlock_release(&queue_lock);
        //     continue; // Skip if the number has already been produced
        // }
        if (atomic_exchange(&passed_numbers[number], 1)) {
            ticketlock_release(&queue_lock);
            continue; // Already produced by another thread
        }
          
        printf("Producer %" PRId64 " generated number: %d\n", id, number); 
        enqueue(&number_queue, number); // Enqueue the number
        // passed_numbers[number] = 1; // Mark the number as produced

        atomic_fetch_add(&produced_count, 1);
        condition_variable_signal(&cv); // Signal the consumer
        ticketlock_release(&queue_lock);
    }

    return NULL;
}


// The "Main" function of the consumer
void* consumer_main(void* arg) {
    (void)arg;
    // Endless loop so each thread keeps producing until all numbers were consumed.
    while(1) {
        int64_t id = (int64_t) pthread_self();

        // Check if finished consuming
        if (atomic_load(&consumed_count) >= MAX_SIZE) {
            return NULL;
        }
        
        // Check the condition variable - if the queue is empty, wait
        // ticketlock_acquire(&queue_lock);
        // while (isEmpty(&number_queue) && atomic_load(&produced_count) < MAX_SIZE) {
        //     condition_variable_wait(&cv, &queue_lock);
        // }

        // Double-check if we should exit after being woken up
        // if (isEmpty(&number_queue) && atomic_load(&consumed_count) >= MAX_SIZE) {
        //     ticketlock_release(&queue_lock);
        //     return NULL;
        // }
        // bool done = isEmpty(&number_queue) && produced_count >= MAX_SIZE;
        // ticketlock_release(&queue_lock);
        // if (done) return NULL;

        // // If the queue is still empty, it might mean all numbers have been produced
        // // and consumed, but we need to check if we should exit
        // if (isEmpty(&number_queue)) {
        //     ticketlock_release(&queue_lock);
        //     continue;
        // }

        ticketlock_acquire(&queue_lock);
        while (isEmpty(&number_queue) && produced_count < MAX_SIZE) {
            condition_variable_wait(&cv, &queue_lock);
        }

        bool done = isEmpty(&number_queue) && produced_count >= MAX_SIZE;
        if (done) {
            ticketlock_release(&queue_lock);
            return NULL;
        }
        ticketlock_release(&queue_lock);

        int number = dequeue(&number_queue);
        printf("Consumer %" PRId64 " checked %d. Is it divisible by 6? %s", id, number, number % 6 == 0 ? "True\n" : "False\n");
    
        atomic_fetch_add(&consumed_count, 1);
        ticketlock_release(&queue_lock);
        }

    return NULL;
}


/*
 * Implements start_consumers_producers.
 * This function should:
 *  - Print the configuration (number of consumers, producers, seed).
 *  - Seed the random number generator using srand().
 *  - Create producer and consumer threads.
 */
void start_consumers_producers(int consumers, int producers, int seed) {
    //Prints configuration and start threads.
    printf("Number of Consumers: %d\n", consumers);
    printf("Number of Producers: %d\n", producers);
    printf("Seed: %d\n", seed);

    srand(seed); // Seed the random number generator

    // Set the global size variables
    producer_array_size = producers;
    consumer_array_size = consumers;

    initqueue(&number_queue); // Initialize the queue
    ticketlock_init(&queue_lock); // Initialize the external lock
    condition_variable_init(&cv); // Initialize the condition variable

    // Initializes the threads' arrays
    producer_threads = malloc(producers * sizeof(pthread_t));
    consumer_threads = malloc(consumers * sizeof(pthread_t));

    for (int i = 0; i < MAX_SIZE; i++) {
        atomic_init(&passed_numbers[i], 0);
    }
    
    // Creating threads
    for (long i = 0; i < producers; i++) {
        pthread_create(&producer_threads[i], NULL, producer_main, NULL);
    }
    for (long i = 0; i < consumers; i++) {
        pthread_create(&consumer_threads[i], NULL, consumer_main, NULL);
    }
}

//Implements stop_consumers to stop all consumers threads.
 
void stop_consumers() {
    // Clean up threads
    for (int i = 0; i < consumer_array_size; i++) {
        pthread_join(consumer_threads[i], NULL); // Wait for each consumer thread to finish
    }
    // Frees the array
    free(consumer_threads);
}

// Added by me
void stop_producers() {
    // Clean up threads
    for (int i = 0; i < producer_array_size; i++) {
        pthread_join(producer_threads[i], NULL); // Wait for each producer thread to finish
    }
    // wake up any consumers that are still waiting
    ticketlock_acquire(&queue_lock);
    condition_variable_broadcast(&cv);
    ticketlock_release(&queue_lock);

    // Frees the array
    free(producer_threads);
}

//Implement print_msg to perform synchronized printing.

/* NOT USED. It is possible to use for printing message like so:
    char msg[100];
    snprintf(msg, sizeof(msg), "Producer %" PRId64 " generated number: %d", id, number);
    print_msg(msg);
*/

void print_msg(const char* msg) {
    // Prints the message ensuring output does not overlap.
    ticketlock_acquire(&queue_lock); // Acquire the external lock
    printf("%s\n", msg); // Print the message
    ticketlock_release(&queue_lock); // Release the external lock
}

/*
 * Implements wait_until_producers_produced_all_numbers 
 * The function should wait until all numbers between 0 and 1,000,000 have been produced.
 */
void wait_until_producers_produced_all_numbers() {
    //Wait until production of numbers (0 to 1,000,000) is complete.
    while (produced_count < MAX_SIZE) {
        sched_yield();
    }
    // Notify consumers that production is complete
    ticketlock_acquire(&queue_lock);
    condition_variable_broadcast(&cv);
    ticketlock_release(&queue_lock);
}

/*
 * Implements wait_consumers_queue_empty to wait until queue is empty, 
 * if queue is already empty - return immediately without waiting.
 */
void wait_consumers_queue_empty() {
    while (produced_count < MAX_SIZE) {
        // Wait until the queue is empty
        sched_yield();
    }
    
    // If the producers are done, busy-wait for consumers
    // while (!isEmpty(&number_queue)) {
    //     sched_yield(); 
    // }
    while (1) {
        ticketlock_acquire(&queue_lock);
        bool empty = isEmpty(&number_queue);
        ticketlock_release(&queue_lock);
        if (empty) break;
        sched_yield();
    }
}

// //Implements a main function that controls the producer-consumer process
// int main(int argc, char* argv[]) {

//     // Check number of arguments
//     if (argc != 4) {
//         fprintf(stderr, "usage: cp_pattern [consumers] [producers] [seed]\n");
//         exit(1);
//     }

//     // Check arguments values are valid
//     int consumers = atoi(argv[1]);
//     int producers = atoi(argv[2]);
//     int seed = atoi(argv[3]);
//     if (consumers <= 0 || producers <= 0 || seed <= 0) {
//         fprintf(stderr, "usage: cp_pattern [consumers] [producers] [seed]\n");
//         exit(1);
//     }
    
//     // Call other functions.
//     start_consumers_producers(consumers, producers, seed);
//     wait_until_producers_produced_all_numbers();
//     stop_producers();
//     wait_consumers_queue_empty();
//     stop_consumers();

//     // Final debug 
//     /*
//     if (produced_count == MAX_SIZE && consumed_count == MAX_SIZE) {
//         printf("SUCCESS!\n");
//     }
//     */

//     return 0;
// }