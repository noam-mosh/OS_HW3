#include "List.h"
#include <stdlib.h>


List createList()
{
    List new_list;
    new_list = (List) malloc(sizeof(*new_list));
    new_list->tail = NULL;
    new_list->head = NULL;
    return new_list;
}


void pushNode(List list, void* data)
{
    if (list->tail != NULL)
    {
        node last = list->tail;
        node tmp = (node) malloc(sizeof(*tmp));
        last->next = tmp;
        tmp->next= NULL;
        tmp->prev = last;
        tmp->data = data;
        list->tail = tmp;

    }
    else
    {
        node tmp = (node) malloc(sizeof(*tmp));
        tmp->next= NULL;
        tmp->prev = NULL;
        tmp->data = data;
        list->head = tmp;
        list->tail = tmp;
    }
}

node popNode(List list)
{
    if (list->head == NULL)
        return NULL;
    node tmp = list->head;
    if (list->tail == list->head)
    {
        list->head = NULL;
        list->tail = NULL;
        return tmp;
    }
    list->head = tmp->next;
    list->head->prev = NULL;
    return tmp;
}

node removeNodeByIndex(List list, int index)
{
    int i=0;
    node curr = list->head;
    while (i < index && curr != NULL)
    {
        curr = curr->next;
        i++;
    }
    if (i == index)
    {
        curr->prev->next = curr->next;
        curr->next->prev = curr->prev;
        return curr;
    }
    return NULL;
}

void removeNodeByData(List list, void* data)
{
    node curr = list->head;
    while (curr != NULL && curr->data !=data)
        curr = curr->next;
    if (curr == NULL)
        return;
    if (curr == list->head)
    {
        list->head = curr->next;
        return;
    }
    if (curr == list->tail)
    {
        list->tail = curr->prev;
        return;
    }
    curr->prev->next = curr->next;
    curr->next->prev = curr->prev;
}

void destroyList(List list)
{
    node curr = list->head;
    while(curr != NULL)
    {
        node next = curr->next;
        free(curr);
        curr = next;
    }
}

