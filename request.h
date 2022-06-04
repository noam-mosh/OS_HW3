#ifndef __REQUEST_H__
#include "Thread.h"
#include "Queue.h"

typedef enum {BLOCK,
              DT,
              RANDOM,
              DH,
              INVALID} Policy;

typedef struct Request_t{
    int fd;
    struct timeval arrive_time;
    Queue handled_q;
    Queue waiting_q;
    Policy policy;
} *Request;

Request CreateRequest(int fd, struct timeval arrive_time, Queue handled_q, Queue waiting_q, Policy policy);
void AddRequest(Request req, Thread* pool,  pthread_mutex_t* global_lock, pthread_cond_t* global_cond);
void requestHandle(int fd);

#endif
