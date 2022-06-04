#ifndef OS_HW3_THREAD_H
#define OS_HW3_THREAD_H
#include "segel.h"

typedef struct Thread_t{
    pthread_t thread;
    int thread_id;
//    int Stat_thread_count;
    int static_request;
    int dynamic_request;
    } Thread;

Thread createThread(pthread_t* thread, void* (*work)(void*), void* arg);

#endif //OS_HW3_THREAD_H
