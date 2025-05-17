#include "queue.h"

// Initialize an empty queue
void initqueue(queue *q) {
    q->head = -1;
    q->tail = -1;
}

// Check if the queue is empty - if so returns 1
int isEmpty(queue *q) {
    return q->head == -1;
}

// Checks if the queue is full - if so returns 1
int isFull(queue *q) {
    return (q->tail + 1) % MAX_SIZE == q->head;
}

// Adds an element to the queue
void enqueue(queue *q, int value) {
    if (isFull(q)) {
        return;
    }
    if (isEmpty(q)) {
        q->head = q->tail = 0;
    } else {
        q->tail = (q->tail + 1) % MAX_SIZE;
    }
    q->data[q->tail] = value;
}

// Dequeues an element from the queue
int dequeue(queue *q) {
    if (isEmpty(q)) {
        return -1;
    }
    int value = q->data[q->head];
    if (q->head == q->tail) {
        q->head = q->tail = -1; // Queue becomes empty
    } else {
        q->head = (q->head + 1) % MAX_SIZE;
    }
    return value;
}

