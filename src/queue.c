#include "../include/queue.h"
#include "../include/ealloc.h"

PriorityQueue *new_queue(int (*compare)(void *, void *))
{
    if (!compare)
        err_quit("no comparator set for queue");

    PriorityQueue *ret = mmalloc(sizeof(PriorityQueue));

    ret->front = NULL;
    ret->compare = compare;
    ret->size = 0;
    return ret;
}

void queue_insert(PriorityQueue *queue, void *data)
{
    if (!queue || !data)
        err_quit("null pointer in queue insert arguments");

    int (*compare)(void *, void *) = queue->compare;
    QueueNode *new_node = mmalloc(sizeof(QueueNode));
    new_node->next = NULL;
    new_node->data = data;

    QueueNode *prev = NULL;
    QueueNode *temp = queue->front;
    while (temp && compare(data, temp->data) < 0) {
        // traverse list until first element that's smaller than or equal
        // to data
        prev = temp;
        temp = temp->next;
    }
    // if nodes have been traversed, there should be a previous node
    if (prev)
        prev->next = new_node;
    else // otherwise place the new node at the front
        queue->front = new_node;
    new_node->next = temp;
    queue->size++;
}

void *queue_pop(PriorityQueue *queue)
{
    if (!queue)
        err_quit("no queue to pop elements from");
    if (queue->size < 1) {
        err_quit("attempted to pop from an empty queue");
    } else if (queue->front == NULL) {
        err_quit("queue size inconsistent with contents");
    }
    QueueNode *detached = queue->front;
    queue->front = detached->next;
    void *ret = detached->data;
    queue->size--;
    free(detached);
    return ret;
}

void *queue_peek(PriorityQueue *queue)
{
    if (!queue)
        err_quit("no queue to pop elements from");
    if (queue->size < 1) {
        err_quit("attempted to peek into an empty queue");
    }
    return queue->front->data;
}

void delete_queue(PriorityQueue *queue)
{
    if (!queue)
        err_quit("no queue to delete");
    QueueNode *cur = queue->front;
    while (cur) {
        QueueNode *prev = cur;
        cur = cur->next;
        free(prev);
    }
    free(queue);
}

size_t queue_size(PriorityQueue *queue)
{
    if (!queue)
        err_quit("no queue");
    return queue->size;
}
