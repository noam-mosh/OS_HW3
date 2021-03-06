//
// request.c: Does the bulk of the work for the web server.
//

#include "segel.h"
#include "request.h"

Request CreateRequest(int fd, Queue handled_q, Queue waiting_q, Policy policy)
{
    Request req = (Request)malloc(sizeof(*req));
    if (!req)
        return NULL;
    req->fd = fd;
    gettimeofday(&req->arrive_time, NULL);
    gettimeofday(&req->dispatch_time, NULL);
//    req->arrive_time = arrival;
    //req->dispatch_time = NULL;  //-NO INITIALIZATION !
    req->handled_q = handled_q;
    req->waiting_q = waiting_q;
    req->policy = policy;
    return req;
}

void updateDispatchTime(Request request) {
    struct timeval dispach;
    gettimeofday(&dispach, NULL);
    timersub(&dispach, &request->arrive_time, &request->dispatch_time);
}

time_t getSec(Request request, int time_type)
{
    if (time_type == ARRIVE)
        return request->arrive_time.tv_sec;
    return request->dispatch_time.tv_sec;
}

time_t getMicroSec(Request request, int time_type)
{
    if (time_type == ARRIVE)
        return request->arrive_time.tv_usec;
    return request->dispatch_time.tv_usec;
}

void AddRequest(Request req, pthread_mutex_t* global_lock, pthread_cond_t* global_cond, int* totalSize)
{
    Request r;
//    Queue handled_req = req->handled_q;
    Queue waiting_req = req->waiting_q;
    pthread_mutex_lock(global_lock);
    if (*totalSize >= waiting_req->maxSize)
    {
        switch (req->policy){
            case BLOCK:
                while (*totalSize >= waiting_req->maxSize)
                    pthread_cond_wait(global_cond, global_lock);
                break;
            case DT:
                Close(req->fd);
                free(req);
                pthread_mutex_unlock(global_lock);
                return;
            case RANDOM:
                //Todo: check case of empty queue
                if (waiting_req->currSize == 0)
                {
                    Close(req->fd);
                    free(req);
                    pthread_mutex_unlock(global_lock);
                    return;
                }
                double tmp = (double)(waiting_req->currSize) * 0.3;
                int num_of_req_to_remove = ceil(tmp);
//                printf("before rand, queue size%d\n", waiting_req->currSize);
//                queue_print(waiting_req);
//                int num_of_req_to_remove = my_ceil(tmp);
                for(int i = 0 ; i < num_of_req_to_remove ; i++)
                {
                    if (waiting_req->currSize == 1)
                    {
                        r = (Request)waiting_req->list->head->data;
                        if (r != NULL)
                        {
                            Close(r->fd);
                            free(r);
                        }
                        free(waiting_req->list->head);
                        waiting_req->list->head = NULL;
                        waiting_req->list->tail = NULL;
                        waiting_req->currSize--;
                    }
                    else{
                        int index_to_remove = rand() % (waiting_req->currSize - 1);
                        r = (Request)dequeue_index(waiting_req, index_to_remove);
                        if (r->fd != -1)
                        {
                            Close(r->fd);
//                        *totalSize = *totalSize - 1;
                            free(r);
                        }
                    }
                }
                *totalSize = *totalSize - (int)num_of_req_to_remove;
                break;
            case DH:
                //Todo: check case of empty queue
                if (waiting_req->currSize == 0)
                {
                    Close(req->fd);
                    free(req);
                    pthread_mutex_unlock(global_lock);
                    return;
                }
                r = (Request)dequeue(waiting_req);
                Close(r->fd);
                free(r);
                *totalSize = *totalSize -1;
                break;
            case INVALID:
                break;
        }
    }
    enqueue(waiting_req, req);
    *totalSize = *totalSize + 1;
    pthread_cond_signal(global_cond);
    pthread_mutex_unlock(global_lock);
}

// requestError(      fd,    filename,        "404",    "Not found", "OS-HW3 Server could not find this file");
void requestError(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg, Request request, Thread thread)
{
    char buf[MAXLINE], body[MAXBUF];

    //updateDispatchTime(request);
    increaseTotalCount(thread);

    // Create the body of the error message
    sprintf(body, "<html><title>OS-HW3 Error</title>");
    sprintf(body, "%s<body bgcolor=""fffff"">\r\n", body);
    sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
    sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
    sprintf(body, "%s<hr>OS-HW3 Web Server\r\n", body);

    // Write out the header information for this response
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    printf("%s", buf);

    sprintf(buf, "Content-Type: text/html\r\n");
    Rio_writen(fd, buf, strlen(buf));
    printf("%s", buf);

    sprintf(buf, "Content-Length: %lu\r\n", strlen(body));
//    sprintf(buf, "%sStat-Req-Arrival:: %lu.%06lu\r\n", buf, thread->curr_request->arrival.tv_sec, thread->curr_request->arrival.tv_usec);
//    sprintf(buf, "%sStat-Req-Dispatch:: %lu.%06lu\r\n", buf, thread->curr_request->dispatched.tv_sec, thread->curr_request->dispatched.tv_usec);
    sprintf(buf, "%sStat-Req-Arrival:: %lu.%06lu\r\n", buf, getSec(request, ARRIVE), getMicroSec(request, ARRIVE));
    sprintf(buf, "%sStat-Req-Dispatch:: %lu.%06lu\r\n", buf, getSec(request, DISPATCH), getMicroSec(request, DISPATCH));
    sprintf(buf, "%sStat-Thread-Id:: %d\r\n", buf, thread->thread_id);
    sprintf(buf, "%sStat-Thread-Count:: %d\r\n", buf, thread->total_request_count);
    sprintf(buf, "%sStat-Thread-Static:: %d\r\n", buf, thread->static_request_count);
    sprintf(buf, "%sStat-Thread-Dynamic:: %d\r\n\r\n", buf, thread->dynamic_request_count);
    Rio_writen(fd, buf, strlen(buf));
    printf("%s", buf);

    // Write out the content
    Rio_writen(fd, body, strlen(body));
    printf("%s", body);

}


//
// Reads and discards everything up to an empty text line
//
void requestReadhdrs(rio_t *rp)
{
    char buf[MAXLINE];

    Rio_readlineb(rp, buf, MAXLINE);
    while (strcmp(buf, "\r\n") != 0) {
        Rio_readlineb(rp, buf, MAXLINE);
    }
}

//
// Return 1 if static, 0 if dynamic content
// Calculates filename (and cgiargs, for dynamic) from uri
//
int requestParseURI(char *uri, char *filename, char *cgiargs)
{
    char *ptr;

    if (strstr(uri, "..")) {
        sprintf(filename, "./public/home.html");
        return 1;
    }

    if (!strstr(uri, "cgi")) {
        // static
        strcpy(cgiargs, "");
        sprintf(filename, "./public/%s", uri);
        if (uri[strlen(uri)-1] == '/') {
            strcat(filename, "home.html");
        }
        return 1;
    } else {
        // dynamic
        ptr = index(uri, '?');
        if (ptr) {
            strcpy(cgiargs, ptr+1);
            *ptr = '\0';
        } else {
            strcpy(cgiargs, "");
        }
        sprintf(filename, "./public/%s", uri);
        return 0;
    }
}

//
// Fills in the filetype given the filename
//
void requestGetFiletype(char *filename, char *filetype)
{
    if (strstr(filename, ".html"))
        strcpy(filetype, "text/html");
    else if (strstr(filename, ".gif"))
        strcpy(filetype, "image/gif");
    else if (strstr(filename, ".jpg"))
        strcpy(filetype, "image/jpeg");
    else
        strcpy(filetype, "text/plain");
}

void requestServeDynamic(int fd, char *filename, char *cgiargs, Request request, Thread thread)
{
    char buf[MAXLINE], *emptylist[] = {NULL};

    //updateDispatchTime(request);
    // The server does only a little bit of the header.
    // The CGI script has to finish writing out the header.
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    sprintf(buf, "%sServer: OS-HW3 Web Server\r\n", buf);

    sprintf(buf, "%sStat-Req-Arrival:: %lu.%06lu\r\n", buf, getSec(request, ARRIVE), getMicroSec(request, ARRIVE));
    sprintf(buf, "%sStat-Req-Dispatch:: %lu.%06lu\r\n", buf, getSec(request, DISPATCH), getMicroSec(request, DISPATCH));
    sprintf(buf, "%sStat-Thread-Id:: %d\r\n", buf, thread->thread_id);
    sprintf(buf, "%sStat-Thread-Count:: %d\r\n", buf, thread->total_request_count);
    sprintf(buf, "%sStat-Thread-Static:: %d\r\n", buf, thread->static_request_count);
    sprintf(buf, "%sStat-Thread-Dynamic:: %d\r\n", buf, thread->dynamic_request_count);

    Rio_writen(fd, buf, strlen(buf));

    pid_t pid = Fork();
    if (pid == 0) {
        /* Child process */
        Setenv("QUERY_STRING", cgiargs, 1);
        /* When the CGI process writes to stdout, it will instead go to the socket */
        Dup2(fd, STDOUT_FILENO);
        Execve(filename, emptylist, environ);
    }
    waitpid(pid, NULL, 0);
}


void requestServeStatic(int fd, char *filename, int filesize, Request request, Thread thread)
{
    int srcfd;
    char *srcp, filetype[MAXLINE], buf[MAXBUF];

    //updateDispatchTime(request);

    requestGetFiletype(filename, filetype);

    srcfd = Open(filename, O_RDONLY, 0);

    // Rather than call read() to read the file into memory,
    // which would require that we allocate a buffer, we memory-map the file
    srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
    Close(srcfd);

    // put together response
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    sprintf(buf, "%sServer: OS-HW3 Web Server\r\n", buf);
    sprintf(buf, "%sContent-Length: %d\r\n", buf, filesize);
    sprintf(buf, "%sContent-Type: %s\r\n", buf, filetype);

    sprintf(buf, "%sStat-Req-Arrival:: %lu.%06lu\r\n", buf, getSec(request, ARRIVE), getMicroSec(request, ARRIVE));
    sprintf(buf, "%sStat-Req-Dispatch:: %lu.%06lu\r\n", buf, getSec(request, DISPATCH), getMicroSec(request, DISPATCH));
    sprintf(buf, "%sStat-Thread-Id:: %d\r\n", buf, thread->thread_id);
    sprintf(buf, "%sStat-Thread-Count:: %d\r\n", buf, thread->total_request_count);
    sprintf(buf, "%sStat-Thread-Static:: %d\r\n", buf, thread->static_request_count);
    sprintf(buf, "%sStat-Thread-Dynamic:: %d\r\n\r\n", buf, thread->dynamic_request_count);

    Rio_writen(fd, buf, strlen(buf));

    //  Writes out to the client socket the memory-mapped file
    Rio_writen(fd, srcp, filesize);
    Munmap(srcp, filesize);

}

// handle a request
void requestHandle(int fd, Request request, Thread thread)
{
    updateDispatchTime(request);
    int is_static;
    struct stat sbuf;
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char filename[MAXLINE], cgiargs[MAXLINE];
    rio_t rio;

    Rio_readinitb(&rio, fd);
    Rio_readlineb(&rio, buf, MAXLINE);
    sscanf(buf, "%s %s %s", method, uri, version);

    printf("%s %s %s\n", method, uri, version);

    if (strcasecmp(method, "GET") != 0) {
        requestError(fd, method, "501", "Not Implemented", "OS-HW3 Server does not implement this method", request, thread);
        return;
    }
    requestReadhdrs(&rio);

    is_static = requestParseURI(uri, filename, cgiargs);
    if (stat(filename, &sbuf) < 0) {
        requestError(fd, filename, "404", "Not found", "OS-HW3 Server could not find this file", request, thread);
        return;
    }

    if (is_static) {
        if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) {
            requestError(fd, filename, "403", "Forbidden", "OS-HW3 Server could not read this file", request, thread);
            return;
        }
        increaseStaticCount(thread);
        requestServeStatic(fd, filename, sbuf.st_size, request, thread);
    } else {
        if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)) {
            requestError(fd, filename, "403", "Forbidden", "OS-HW3 Server could not run this CGI program", request, thread);
            return;
        }
        increaseDynamicCount(thread);
        requestServeDynamic(fd, filename, cgiargs, request, thread);
    }
}
