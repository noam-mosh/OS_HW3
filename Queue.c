#include "Queue.h"

Queue createQueue(size_t maxSize, pthread_mutex_t* lock, pthread_cond_t* cond_enc, pthread_cond_t* cond_dec) {
    Queue q = NULL;
    q = (Queue)malloc(sizeof(*q));
    if (!q)
        return NULL;
    q->list = createList();
    q->lock = lock;
    q->cond_for_enqueue = cond_enc;
    q->cond_for_dequeue = cond_dec;
    pthread_mutex_init((q->lock), NULL);
    pthread_cond_init((q->cond_for_enqueue), NULL);
    pthread_cond_init((q->cond_for_dequeue), NULL);
    return q;
}

void destroyQueue(Queue q){
    pthread_mutex_destroy((q->lock));
    pthread_cond_destroy((q->cond_for_enqueue));
    pthread_cond_destroy((q->cond_for_dequeue));
    destroyList(q->list);
    free(q);
}

errorType enqueue(Queue q, void* data) {
    if (!q || !data)
        return NULL_ARGUMENT;
    pthread_mutex_lock(q->lock);
    while (q->currSize == q->maxSize) {
        pthread_cond_wait(q->cond_for_enqueue, q->lock);
    }
    pushNode(q->list, data);
    q->currSize++;
    pthread_cond_signal(q->cond_for_dequeue);
    pthread_mutex_unlock(q->lock);
}

errorType dequeue(Queue q) {
    if (!q)
        return NULL_ARGUMENT;
    pthread_mutex_lock(q->lock);
    while (q->currSize == 0) {
        pthread_cond_wait(q->cond_for_dequeue, q->lock);
    }
    popNode(q->list);
    q->currSize--;
    pthread_cond_signal(q->cond_for_enqueue);
    pthread_mutex_unlock(q->lock);
}

size_t getQueueSize(Queue q)
{
    return q->currSize;
}


