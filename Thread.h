#ifndef OS_HW3_THREAD_H
#define OS_HW3_THREAD_H
#include "segel.h"
#include "Queue.h"

typedef struct Thread_t{
    pthread_t* thread;
    Queue handled_q;
    Queue waiting_q;
    int thread_id;
    int static_request_count;
    int dynamic_request_count;
    int total_request_count
    } *Thread;

Thread createThread(int thread_id, pthread_t* thread, Queue handled_q, Queue waiting_q, void* (*work)(void*), void* arg);
void increaseStaticCount (Thread thread);
void increaseDynamicCount (Thread thread);
void increaseTotalCount (Thread thread);
#endif //OS_HW3_THREAD_H
