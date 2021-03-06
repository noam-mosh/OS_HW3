#include "Queue.h"


Queue createQueue(size_t maxSize, pthread_mutex_t* lock, pthread_cond_t* cond_enc, pthread_cond_t* cond_dec) {
    Queue q;
    q = (Queue)malloc(sizeof(*q));
    if (q==NULL)
        return q;
    q->list = createList();
    q->currSize = 0;
    q->maxSize = maxSize;
    q->lock = lock;
    q->enqueue_allowed = cond_enc;
    q->dequeue_allowed = cond_dec;
    pthread_mutex_init((q->lock), NULL);
    pthread_cond_init((q->enqueue_allowed), NULL);
    pthread_cond_init((q->dequeue_allowed), NULL);
    return q;
}

void destroyQueue(Queue q){
    pthread_mutex_destroy((q->lock));
    pthread_cond_destroy((q->enqueue_allowed));
    pthread_cond_destroy((q->dequeue_allowed));
    destroyList(q->list);
    free(q);
}

void enqueue(Queue q, void* data) {
    if (!q || !data)
        return;
    pthread_mutex_lock(q->lock);
    while (q->currSize >= q->maxSize) {
        pthread_cond_wait(q->enqueue_allowed, q->lock);
    }
    pushNode(q->list, data);
    q->currSize++;
    pthread_cond_signal(q->dequeue_allowed);
    pthread_mutex_unlock(q->lock);
}

void* dequeue(Queue q) {
    //Todo: add case for when queue is empty
    if (!q)
        return NULL;
    pthread_mutex_lock(q->lock);
    while (q->currSize == 0) {
        pthread_cond_wait(q->dequeue_allowed, q->lock);
    }
    node delete = popNode(q->list);
    q->currSize--;
    pthread_cond_signal(q->enqueue_allowed);
    pthread_mutex_unlock(q->lock);
    return delete->data;
}

void removeQueue(Queue q, void* data)
{
    //Todo: add case for when queue is empty
    if (!q)
        return;
    pthread_mutex_lock(q->lock);
    while (q->currSize == 0) {
        pthread_cond_wait(q->dequeue_allowed, q->lock);
    }
    removeNodeByData(q->list, data);
    q->currSize--;
    pthread_cond_signal(q->enqueue_allowed);
    pthread_mutex_unlock(q->lock);
}

void* dequeue_index(Queue q, int index)
{
    //Todo: add case for when queue is empty
    if (!q)
        return (void *) NULL_ARGUMENT;
    pthread_mutex_lock(q->lock);
    while (q->currSize == 0) {
        pthread_cond_wait(q->dequeue_allowed, q->lock);
    }
    node ret = removeNodeByIndex(q->list, index);
    q->currSize--;
    pthread_cond_signal(q->enqueue_allowed);
    pthread_mutex_unlock(q->lock);
    return ret->data;
}

//void queue_print(Queue queue, void* info)
//{
//    if (queue->currSize == 0)
//    {
//        printf("Queue is empty\n");
//        return;
//    }
//
//    node temp = queue->list->head;
//    while (temp)
//    {
//        printf("%d-> ", temp->data->info);
//        temp = temp->next;
//    }
//    printf("\n");
//}

