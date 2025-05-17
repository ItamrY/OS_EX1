#include "cp_pattern.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

// Basic test to validate functionality and edge cases

void test_basic_functionality(int consumers, int producers, int seed) {
    printf("\n=== Test: Basic Functionality (C=%d, P=%d, Seed=%d) ===\n", consumers, producers, seed);
    start_consumers_producers(consumers, producers, seed);
    wait_until_producers_produced_all_numbers();
    printf("[Test] All producers finished.\n");
    wait_consumers_queue_empty();
    printf("[Test] Consumer queue is empty.\n");
    stop_consumers();
    printf("[Test] All consumers stopped.\n");
}

void test_edge_case_zero_seed() {
    printf("\n=== Test: Edge Case - Seed = 0 ===\n");
    start_consumers_producers(2, 2, 0);
    wait_until_producers_produced_all_numbers();
    printf("[Test] All producers finished.\n");
    wait_consumers_queue_empty();
    printf("[Test] Consumer queue is empty.\n");
    stop_consumers();
    printf("[Test] All consumers stopped.\n");
}

void test_stress_case() {
    printf("\n=== Test: Stress Case (Many Threads) ===\n");
    start_consumers_producers(50, 50, time(NULL));
    wait_until_producers_produced_all_numbers();
    printf("[Test] All producers finished.\n");
    wait_consumers_queue_empty();
    printf("[Test] Consumer queue is empty.\n");
    stop_consumers();
    printf("[Test] All consumers stopped.\n");
}

void test_basic_small_threads() {
    printf("\n=== Test: Small Config (1 Consumer, 1 Producer) ===\n");
    start_consumers_producers(1, 1, 1);
    wait_until_producers_produced_all_numbers();
    printf("[Test] All producers finished.\n");
    wait_consumers_queue_empty();
    printf("[Test] Consumer queue is empty.\n");
    stop_consumers();
    printf("[Test] All consumers stopped.\n");
}

void test_basic_uneven_threads() {
    printf("\n=== Test: More Consumers than Producers (C=5, P=2) ===\n");
    start_consumers_producers(5, 2, 123);
    wait_until_producers_produced_all_numbers();
    printf("[Test] All producers finished.\n");
    wait_consumers_queue_empty();
    printf("[Test] Consumer queue is empty.\n");
    stop_consumers();
    printf("[Test] All consumers stopped.\n");
}

void test_basic_more_producers() {
    printf("\n=== Test: More Producers than Consumers (C=2, P=5) ===\n");
    start_consumers_producers(2, 5, 456);
    wait_until_producers_produced_all_numbers();
    printf("[Test] All producers finished.\n");
    wait_consumers_queue_empty();
    printf("[Test] Consumer queue is empty.\n");
    stop_consumers();
    printf("[Test] All consumers stopped.\n");
}

void test_heavy_unbalanced() {
    printf("\n=== Test: Heavy Unbalanced (C=3, P=30) ===\n");
    start_consumers_producers(3, 30, 999);
    wait_until_producers_produced_all_numbers();
    printf("[Test] All producers finished.\n");
    wait_consumers_queue_empty();
    printf("[Test] Consumer queue is empty.\n");
    stop_consumers();
    printf("[Test] All consumers stopped.\n");
}

void test_heavy_consumers_flood() {
    printf("\n=== Test: Heavy Consumers Flood (C=40, P=2) ===\n");
    start_consumers_producers(40, 2, 2024);
    wait_until_producers_produced_all_numbers();
    printf("[Test] All producers finished.\n");
    wait_consumers_queue_empty();
    printf("[Test] Consumer queue is empty.\n");
    stop_consumers();
    printf("[Test] All consumers stopped.\n");
}

int main() {
    printf("===== Starting cp_pattern Tests =====\n");

    test_basic_functionality(4, 4, 42);
    test_basic_small_threads();
    test_basic_uneven_threads();
    test_basic_more_producers();

    test_edge_case_zero_seed();

    test_heavy_unbalanced();
    test_heavy_consumers_flood();

    test_stress_case();

    printf("===== All Tests Completed =====\n");
    return 0;

    // if (produced_count == MAX_SIZE && consumed_count == MAX_SIZE) {
    // printf("SUCCESS: All values produced and consumed.\n");
    // }

}
