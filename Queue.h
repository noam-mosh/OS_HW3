#ifndef WEBSERVER_FILES_QUEUE_H
#define WEBSERVER_FILES_QUEUE_H

#include "segel.h"
#include "List.h"

typedef enum {OUT_OF_MEMORY,
    NULL_ARGUMENT,
    QUEUE_EMPTY,
    NOT_FOUND,
    SUCCESS} errorType;

typedef struct Queue_t{
    List list;
    pthread_mutex_t* lock;
    pthread_cond_t* enqueue_allowed;
    pthread_cond_t* dequeue_allowed;
    size_t currSize; // The current size
    size_t maxSize; // The maximal size
} *Queue;

Queue createQueue(size_t maxSize, pthread_mutex_t* lock, pthread_cond_t* cond_enc, pthread_cond_t* cond_dec);
void destroyQueue(Queue q);
void enqueue(Queue q, void* data);
void* dequeue(Queue q);
void removeQueue(Queue q, void* data);
void* dequeue_index(Queue q, int index);

#endif //WEBSERVER_FILES_QUEUE_H