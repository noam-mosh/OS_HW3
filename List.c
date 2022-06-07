#include "List.h"
#include <stdlib.h>


List createList()
{
    List new_list;
    new_list = (List) malloc(sizeof(List));
    new_list->tail = NULL;
    new_list->head = NULL;
    return new_list;
}

//    node tmp; // declare a node
//    tmp = (node) malloc(sizeof(node));
//    tmp->next = NULL;// make next point to NULL
//    tmp->prev = NULL;
//    new_list->tail = tmp;
//    new_list->head = tmp;
//    return new_list;//return the new node
//}

void pushNode(List list, void* data)
{
    node last = list->tail;
    node tmp = (node) malloc(sizeof(node));
    last->next = tmp;
    tmp->next= NULL;
    tmp->prev = last;
    tmp->data = data;

    if (list->head == NULL)
        list->head = tmp;
    list->tail = tmp;
}

node popNode(List list)
{
    if (list->head == NULL)
        return NULL;
    node tmp = list->head;
    if (list->tail == list->head)
    {
        free(list->head);
        free(list->tail);
        list->head = NULL;
        list->tail = NULL;
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

node removeNodeByData(List list, void* data)    //TODO: void?
{
    node curr = list->head;
    while (curr != NULL && curr->data !=data)
        curr = curr->next;
    if (curr == NULL)
        return NULL;
    curr->prev->next = curr->next;
    curr->next->prev = curr->prev;
    return curr;
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