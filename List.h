//
// Created by belen on 03/06/2022.
//

#ifndef LIST_C_LIST_H
#define LIST_C_LIST_H


typedef struct node_t
{
    void* data;
    struct node_t *next;
    struct node_t *prev;
}*node;

typedef struct List_t
{
    node head;
    node tail;
}*List;

List createList();
void pushNode(List list, void* data);
void popNode(List list);
void destroyList(List list);

//typedef struct node_t *node;

//typedef struct List_t *List;

//struct node createNode();

//typedef struct Node *node; //Define node as pointer of data type struct LinkedList

#endif //LIST_C_LIST_H
