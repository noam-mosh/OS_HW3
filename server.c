#include "segel.h"
#include "request.h"
#include "Queue.h"
#include "Thread.h"
// 
// server.c: A very, very simple web server
//
// To run:
//  ./server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//

// HW3: Parse the new arguments too
void getargs(int* port, int* threads_num, int* queue_size, Policy* schedalg, int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    *port = atoi(argv[1]);
    *threads_num = atoi(argv[2]);
    *queue_size = atoi(argv[3]);

    if (strcmp(argv[4], "block") == 0)
        *schedalg = BLOCK;

    else if (strcmp(argv[4], "dh") == 0)
        *schedalg =  DH;

    else if (strcmp(argv[4], "dt") == 0)
        *schedalg = DT;

    else if (strcmp(argv[4], "random") == 0)
        *schedalg = RANDOM;

    else
        *schedalg = INVALID;
}

//void worker(Thread thread)
//{
//    while (1)
//    {
//        Request r = (Request) dequeue(thread.waiting_q);
//        enqueue(thread.handled_q, r);
//        requestHandle(r->fd);
//        removeQueue(thread.handled_q, r);
//        Close(r->fd);
//        free(r);
//    }
//}

void* start_routine(void* thread) {
    Thread tmp = (Thread)thread;
    while (1)
    {
        Request r = (Request) dequeue(tmp->waiting_q);
        enqueue(tmp->handled_q, r);
        requestHandle(r->fd, r, tmp);
        removeQueue(tmp->handled_q, r);
        Close(r->fd);
        free(r);
    }
}

int main(int argc, char *argv[])
{
    int listenfd, connfd, port, threads_num, queue_size, clientlen;
    struct sockaddr_in clientaddr;
    Policy schedalg;
    getargs(&port, &threads_num, &queue_size, &schedalg, argc, argv);

    pthread_mutex_t waiting_lock = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t handled_lock = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t global_lock = PTHREAD_MUTEX_INITIALIZER;

    pthread_cond_t waiting_cond_enc = PTHREAD_COND_INITIALIZER;
    pthread_cond_t waiting_cond_dec = PTHREAD_COND_INITIALIZER;
    pthread_cond_t handled_cond_enc = PTHREAD_COND_INITIALIZER;
    pthread_cond_t handled_cond_dec = PTHREAD_COND_INITIALIZER;
    pthread_cond_t global_cond = PTHREAD_COND_INITIALIZER;

    Queue handled_q = createQueue(threads_num, 0, &handled_lock, &handled_cond_enc, &handled_cond_dec);
    Queue waiting_q = createQueue(queue_size, 0, &waiting_lock, &waiting_cond_enc, &waiting_cond_dec);
    // 
    // HW3: Create some threads...
    //
    Thread* threads_pull = (Thread*) malloc(threads_num * sizeof(Thread));
    if (!threads_pull)
        return NULL;
    for (int i = 0; i < threads_num; ++i) {
        threads_pull[i] = createThread(i, threads_pull[i]->thread, handled_q, waiting_q, start_routine, threads_pull[i]->thread);
    }

    listenfd = Open_listenfd(port);
    while (1) {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);

        //
        // HW3: In general, don't handle the request in the main thread.
        // Save the relevant info in a buffer and have one of the worker threads
        // do the work.
        //
        struct timeval arrival;
        gettimeofday(&arrival, NULL);
        Request r = CreateRequest(connfd, arrival, handled_q, waiting_q, schedalg);
        AddRequest(r, threads_pull, &global_lock, &global_cond);
//        requestHandle(connfd);

//        Close(connfd);
    }
    destroyQueue(handled_q);
    destroyQueue(waiting_q);
}


    


 
