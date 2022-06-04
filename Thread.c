#include "Thread.h"

Thread createThread(pthread_t* thread, Queue handled_q, Queue waiting_q, void* (*work)(void*), void* arg)
{
    Thread t = (Thread)malloc(sizeof(Thread));
    if (!t)
        return NULL;
    t->thread = thread;
    t->handled_q = handled_q;
    t->waiting_q = waiting_q;
//    t->thread_id = thread_id;
//    t->static_request = static_request;
//    t->dynamic_request = dynamic_request;
    if(pthread_create(thread, NULL ,work, &arg))
    {
        return NULL;
    }
    return t;
}