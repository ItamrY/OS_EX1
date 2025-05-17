#ifndef QUEUE_H
#define QUEUE_H


//#define MAX_SIZE 1000000
#define MAX_SIZE 100 // Maximum size of numbers in the queue

typedef struct {
    int data[MAX_SIZE]; 
    int head;
    int tail;
} queue;

void initqueue(queue *q);

int isEmpty(queue *q);

int isFull(queue *q);

void enqueue(queue *q, int value);

int dequeue(queue *q);



#endif // QUEUE_H