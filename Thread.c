#include "Thread.h"

Thread createThread(pthread_t* thread, void* (*work)(void*), void* arg)
{
    if(pthread_create(thread, NULL ,work, &arg))
    {
        return NULL;
    }
}