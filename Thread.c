#include "Thread.h"

Thread createThread(int thread_id, Queue handled_q, Queue waiting_q)
{
    Thread t = (Thread)malloc(sizeof(Thread));
    if (!t)
        return NULL;
    t->handled_q = handled_q;
    t->waiting_q = waiting_q;
    t->thread_id = thread_id;
    t->static_request_count = 0;
    t->dynamic_request_count = 0;
    t->total_request_count = 0;
    return t;
}

int activateTread(Thread thread, void* (*work)(void* arg)) {
    return (pthread_create(&(thread->thread), NULL, work, thread));
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