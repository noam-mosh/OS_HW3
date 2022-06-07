//
// request.c: Does the bulk of the work for the web server.
// 

#include "segel.h"
#include "request.h"

Request CreateRequest(int fd, struct timeval arrive_time, Queue handled_q, Queue waiting_q, Policy policy)
{
    Request req = (Request)malloc(sizeof(Request));
    if (!req)
        return NULL;
    req->fd = fd;
    req->arrive_time = arrive_time;
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

void AddRequest(Request req, Thread* pool, pthread_mutex_t* global_lock, pthread_cond_t* global_cond)
{
    int fd, num_of_req_to_remove, empty = 1;
    Queue handled_req = req->handled_q;
    Queue waiting_req = req->waiting_q;
    pthread_mutex_lock(global_lock);
    if (handled_req->currSize + waiting_req->currSize >= waiting_req->maxSize)
    {
        switch (req->policy){
            case BLOCK:
                while (handled_req->currSize + waiting_req->currSize >= waiting_req->maxSize)
                    pthread_cond_wait(global_cond, global_lock);
                break;
            case DT:
                Close(req->fd);
                pthread_mutex_unlock(global_lock);
                return;
            case RANDOM:
                num_of_req_to_remove = (int)((double)(waiting_req->currSize) * 0.3);
                for(int i = 0 ; i < num_of_req_to_remove ; i++)
                {
                    empty = 0;
                    fd = dequeue_index(waiting_req, abs(rand())%(waiting_req->currSize));
                    Close(fd);
                }
                if (empty)
                {
                    Close(req->fd);
                    pthread_mutex_unlock(global_lock);
                    return;
                }
                break;
            case DH:
                fd = dequeue(waiting_req);
                //Todo: add case for when queue is empty
                Close(fd);
                break;
            case INVALID:
                break;
        }
    }
    enqueue(waiting_req, &req->fd);
    pthread_cond_signal(global_cond);
    pthread_mutex_unlock(global_lock);
}

// requestError(      fd,    filename,        "404",    "Not found", "OS-HW3 Server could not find this file");
void requestError(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg, Request request, Thread thread)
{
   char buf[MAXLINE], body[MAXBUF];

    updateDispatchTime(request);
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

    sprintf(buf, "Content-Length: %lu\r\n\r\n", strlen(body));
    Rio_writen(fd, buf, strlen(buf));
    printf("%s", buf);

    sprintf(buf, "%sStat-Req-Arrival:: %lu.%06lu\r\n", getSec(request, ARRIVE), getMicroSec(request, ARRIVE));
    sprintf(buf, "%sStat-Req-Dispatch:: %lu.%06lu\r\n", getSec(request, DISPATCH), getMicroSec(request, DISPATCH));
    sprintf(buf, "%sStat-Thread-Id:: %d\r\n", thread->thread_id);
    sprintf(buf, "%sStat-Thread-Count:: %d\r\n", thread->total_request_count);
    sprintf(buf, "%sStat-Thread-Static:: %d\r\n", thread->static_request_count);
    sprintf(buf, "%sStat-Thread-Dynamic:: %d\r\n\r\n", thread->dynamic_request_count);

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
    while (strcmp(buf, "\r\n")) {
        Rio_readlineb(rp, buf, MAXLINE);
    }
    return;
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

    updateDispatchTime(request);
    increaseDynamicCount(thread);
    // The server does only a little bit of the header.
    // The CGI script has to finish writing out the header.
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    sprintf(buf, "%sServer: OS-HW3 Web Server\r\n", buf);

    sprintf(buf, "%sStat-Req-Arrival:: %lu.%06lu\r\n", getSec(request, ARRIVE), getMicroSec(request, ARRIVE));
    sprintf(buf, "%sStat-Req-Dispatch:: %lu.%06lu\r\n", getSec(request, DISPATCH), getMicroSec(request, DISPATCH));
    sprintf(buf, "%sStat-Thread-Id:: %d\r\n", thread->thread_id);
    sprintf(buf, "%sStat-Thread-Count:: %d\r\n", thread->total_request_count);
    sprintf(buf, "%sStat-Thread-Static:: %d\r\n", thread->static_request_count);
    sprintf(buf, "%sStat-Thread-Dynamic:: %d\r\n\r\n", thread->dynamic_request_count);

    Rio_writen(fd, buf, strlen(buf));

    if (Fork() == 0) {
        /* Child process */
        Setenv("QUERY_STRING", cgiargs, 1);
        /* When the CGI process writes to stdout, it will instead go to the socket */
        Dup2(fd, STDOUT_FILENO);
        Execve(filename, emptylist, environ);
    }
    Wait(NULL);
}


void requestServeStatic(int fd, char *filename, int filesize, Request request, Thread thread)
{
    int srcfd;
    char *srcp, filetype[MAXLINE], buf[MAXBUF];

    updateDispatchTime(request);
    increaseStaticCount(thread);

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
    sprintf(buf, "%sContent-Type: %s\r\n\r\n", buf, filetype);

    sprintf(buf, "%sStat-Req-Arrival:: %lu.%06lu\r\n", getSec(request, ARRIVE), getMicroSec(request, ARRIVE));
    sprintf(buf, "%sStat-Req-Dispatch:: %lu.%06lu\r\n", getSec(request, DISPATCH), getMicroSec(request, DISPATCH));
    sprintf(buf, "%sStat-Thread-Id:: %d\r\n", thread->thread_id);
    sprintf(buf, "%sStat-Thread-Count:: %d\r\n", thread->total_request_count);
    sprintf(buf, "%sStat-Thread-Static:: %d\r\n", thread->static_request_count);
    sprintf(buf, "%sStat-Thread-Dynamic:: %d\r\n\r\n", thread->dynamic_request_count);

    Rio_writen(fd, buf, strlen(buf));

    //  Writes out to the client socket the memory-mapped file
    Rio_writen(fd, srcp, filesize);
    Munmap(srcp, filesize);

}

// handle a request
void requestHandle(int fd, Request request, Thread thread)
{
    int is_static;
    struct stat sbuf;
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char filename[MAXLINE], cgiargs[MAXLINE];
    rio_t rio;

    Rio_readinitb(&rio, fd);
    Rio_readlineb(&rio, buf, MAXLINE);
    sscanf(buf, "%s %s %s", method, uri, version);

    printf("%s %s %s\n", method, uri, version);

    if (strcasecmp(method, "GET")) {
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
        requestServeStatic(fd, filename, sbuf.st_size, request, thread);
    } else {
        if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)) {
            requestError(fd, filename, "403", "Forbidden", "OS-HW3 Server could not run this CGI program", request, thread);
            return;
        }
        requestServeDynamic(fd, filename, cgiargs, request, thread);
    }
}


