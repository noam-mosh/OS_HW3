#include "segel.h"
#include "request.h"

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
void getargs(int* port, int* threads_num, int* queue_size, char** schedalg, int argc, char *argv[])
{
    if (argc < 2) {
	fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	exit(1);
    }
    *port = atoi(argv[1]);
    *threads_num = atoi(argv[2]);
    *queue_size = atoi(argv[3]);
    *schedalg = argv[4];
}

void worker()
{

}

int main(int argc, char *argv[])
{
    int listenfd, connfd, port, threads_num, queue_size, clientlen;
    struct sockaddr_in clientaddr;

    getargs(&port, &threads_num, &queue_size, schedalg, argc, argv);
    pthread_mutex_t handled_lock = PTHREAD_MUTEX_INITIALIZER, waiting_lock = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t waiting_cond_enc = PTHREAD_COND_INITIALIZER, waiting_cond_dec = PTHREAD_COND_INITIALIZER;
    pthread_cond_t handled_cond_enc = PTHREAD_COND_INITIALIZER, handled_cond_dec = PTHREAD_COND_INITIALIZER;

    Queue handled_req = createQueue(threads_num, &handled_lock, &handled_cond_enc, &handled_cond_dec);
    Queue waiting_req = createQueue(queue_size, &waiting_lock, &waiting_cond_enc, &waiting_cond_dec);
    // 
    // HW3: Create some threads...
    //
    listenfd = Open_listenfd(port);
    while (1) {
	clientlen = sizeof(clientaddr);
	connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);

	// 
	// HW3: In general, don't handle the request in the main thread.
	// Save the relevant info in a buffer and have one of the worker threads 
	// do the work. 
	//

    for (int i = 0; i < ; ++i) {

        }
	requestHandle(connfd);

	Close(connfd);
    }

}


    


 
