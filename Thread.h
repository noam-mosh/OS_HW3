#ifndef OS_HW3_THREAD_H
#define OS_HW3_THREAD_H
#include "segel.h"
#include "Queue.h"

typedef struct Thread_t{
    unsigned int thread_id;
    pthread_t thread;
    Queue handled_q;
    Queue waiting_q;
    int static_request_count;
    int dynamic_request_count;
    int total_request_count;
    pthread_mutex_t* global_lock;
    pthread_cond_t* global_cond;
    int* totalSize;
} *Thread;

Thread createThread(unsigned int thread_id, Queue handled_q, Queue waiting_q, pthread_mutex_t* global_lock, pthread_cond_t* global_cond, int* totalSize);
int activateTread(Thread thread, void* (*work)(void* arg));
void increaseStaticCount (Thread thread);
void increaseDynamicCount (Thread thread);
void increaseTotalCount (Thread thread);

#endif //OS_HW3_THREAD_H
