#include "Thread.h"
#include <pthread.h>
Thread createThread(unsigned int thread_id, Queue handled_q, Queue waiting_q, pthread_mutex_t* global_lock, pthread_cond_t* global_cond, int* totalSize)
{
    Thread t = (Thread)malloc(sizeof(*t));
    if (!t)
        return NULL;
    t->handled_q = handled_q;
    t->waiting_q = waiting_q;
    t->thread_id = thread_id;
    t->curr_request = NULL;
    t->static_request_count = 0;
    t->dynamic_request_count = 0;
    t->total_request_count = 0;
    t->global_lock = global_lock;
    t->global_cond = global_cond;
    t->totalSize = totalSize;
    return t;
}

int activateTread(Thread thread, void* (*work)(void* arg)) {
    int res = pthread_create(&(thread->thread), NULL, work, thread);
    return (res);
}

void increaseStaticCount (Thread thread)
{
    thread->static_request_count++;
    thread->total_request_count++;
}

void increaseDynamicCount (Thread thread)
{
    thread->dynamic_request_count++;
    thread->total_request_count++;
}
void increaseTotalCount (Thread thread)
{
    thread->total_request_count++;
}