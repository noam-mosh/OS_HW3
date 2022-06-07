#ifndef WEBSERVER_FILES_QUEUE_H
#define WEBSERVER_FILES_QUEUE_H

#include "segel.h"
#include "List.h"

typedef enum {OUT_OF_MEMORY,
              NULL_ARGUMENT,
              QUEUE_EMPTY,
              NOT_FOUND} errorType;

typedef struct Queue_t{
    List list;
    pthread_mutex_t* lock;
    pthread_cond_t* enqueue_allowed;
    pthread_cond_t* dequeue_allowed;
    size_t currSize; // The current size
    size_t maxSize; // The maximal size
    size_t totalSize; // The total size
} *Queue;

Queue createQueue(size_t maxSize, size_t totalSize, pthread_mutex_t* lock, pthread_cond_t* cond_enc, pthread_cond_t* cond_dec);
void destroyQueue(Queue q);
errorType enqueue(Queue q, void* data);
errorType dequeue(Queue q);
errorType removeQueue(Queue q, void* data);
void* dequeue_index(Queue q, int index);
size_t getQueueSize(Queue q);
size_t getQueueTotalSize(Queue q);


#endif //WEBSERVER_FILES_QUEUE_H