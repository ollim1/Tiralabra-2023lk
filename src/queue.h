#ifndef QUEUE_H
#define QUEUE_H
#include "error.h"
#include <stdlib.h>
#include <sys/types.h>

typedef struct queuenode_st QueueNode;
struct queuenode_st {
    QueueNode *next;
    void *data;
};
typedef struct priorityqueue_st {
    /*
     * Priority queue implemented as linked list
     * biggest element first by default, so works as a max heap
     */
    QueueNode *front;
    int (*compare)(void *, void *);
    ssize_t size;
} PriorityQueue;

PriorityQueue *new_queue(int (*)(void *, void *));
void queue_insert(PriorityQueue *, void *);
void *queue_pop(PriorityQueue *);
void *queue_peek(PriorityQueue *);
void delete_queue(PriorityQueue *);
size_t queue_size(PriorityQueue *);

#endif
