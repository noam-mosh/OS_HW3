#ifndef OS_HW3_THREAD_H
#define OS_HW3_THREAD_H
#include "segel.h"
#include "Queue.h"

typedef struct Thread_t{
    pthread_t* thread;
    Queue handled_q;
    Queue waiting_q;
//    int thread_id;
    int static_request;
    int dynamic_request;
    } *Thread;

Thread createThread(pthread_t* thread, Queue handled_q, Queue waiting_q, void* (*work)(void*), void* arg);

#endif //OS_HW3_THREAD_H
