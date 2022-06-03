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

void popNode(List list)
{
    if (list->tail == NULL)
        return;
    if (list->tail == list->head)
    {
        free(list->head);
        free(list->tail);
        list->head = NULL;
        list->tail = NULL;
    }
    node tmp = list->tail;
    list->tail = tmp->prev;
    list->tail->next = NULL;
    free(tmp);
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