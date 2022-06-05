#include "Thread.h"

Thread createThread(int thread_id, pthread_t* thread, Queue handled_q, Queue waiting_q, void* (*work)(void*), void* arg)
{
    Thread t = (Thread)malloc(sizeof(Thread));
    if (!t)
        return NULL;
    t->thread = thread;
    t->handled_q = handled_q;
    t->waiting_q = waiting_q;
    t->thread_id = thread_id;
    t->static_request_count = 0;
    t->dynamic_request_count = 0;
    t->total_request_count = 0;
    if(pthread_create(thread, NULL ,work, &arg))
    {
        return NULL;
    }
    return t;
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