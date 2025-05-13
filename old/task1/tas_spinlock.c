#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <threads.h>

typedef struct {
    atomic_flag lock;
} spinlock_t;

void spinlock_init(spinlock_t *spinlock) {
    atomic_flag_clear(&spinlock->lock);
}

void spinlock_acquire(spinlock_t *spinlock) {
    while (atomic_flag_test_and_set(&spinlock->lock)) {
        // Busy-wait until the lock is released
    }
}

void spinlock_release(spinlock_t *spinlock) {
    atomic_flag_clear(&spinlock->lock);
}

// Example usage
int shared_resource = 0;
spinlock_t spinlock;

int thread_func(void *arg) {
    for (int i = 0; i < 100000; i++) {
        spinlock_acquire(&spinlock);
        shared_resource++;
        spinlock_release(&spinlock);
    }
    return 0;
}

int main() {
    spinlock_init(&spinlock);

    thrd_t t1, t2;
    thrd_create(&t1, thread_func, NULL);
    thrd_create(&t2, thread_func, NULL);

    thrd_join(t1, NULL);
    thrd_join(t2, NULL);

    printf("Final value of shared_resource: %d\n", shared_resource);
    return 0;
}