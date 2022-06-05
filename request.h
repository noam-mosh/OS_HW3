#ifndef __REQUEST_H__
#include "Thread.h"
#include "Queue.h"

typedef enum {BLOCK,
              DT,
              RANDOM,
              DH,
              INVALID} Policy;

typedef enum {ARRIVE,
              DISPATCH} Time_type;

typedef struct Request_t{
    int fd;
    struct timeval arrive_time;
    struct timeval dispatch_time;
    Queue handled_q;
    Queue waiting_q;
    Policy policy;
} *Request;

Request CreateRequest(int fd, struct timeval arrive_time, Queue handled_q, Queue waiting_q, Policy policy);
void AddRequest(Request req, Thread* pool,  pthread_mutex_t* global_lock, pthread_cond_t* global_cond);
void requestHandle(int fd, Request request, Thread thread);

void updateDispatchTime(Request request);
time_t getSec(Request request, int time_type);
time_t getMicroSec(Request request, int time_type);
#endif
