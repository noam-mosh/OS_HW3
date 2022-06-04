#ifndef __REQUEST_H__
#include "Thread.h"
#include "Queue.h"

typedef enum {BLOCK,
              DT,
              RANDOM,
              DH,
              INVALID} Policy;

typedef struct Request_t{
} *Request;

void createRequest(int connfd, Thread* pool, Queue handled_req, Queue waiting_req, Policy policy, pthread_mutex_t* global_lock, pthread_cond_t* global_cond);
void requestHandle(int fd);

#endif
