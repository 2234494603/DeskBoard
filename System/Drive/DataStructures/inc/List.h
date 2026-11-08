#ifndef _LIST_H_
#define _LIST_H_

#include "System_config.h"

typedef struct List 
{
    int data ;//头节点

    struct List* prev ;//前进指针
    struct List* next ;//后退指针
}List_Struct;


void List_Iint(List_Struct* prev,int num);
void List_PrintForward(List_Struct* head);
void List_PrintBackward(List_Struct* head);
List_Struct* List_PushFront(List_Struct* head, int num);
List_Struct* List_PushBack(List_Struct* head, int num);
List_Struct* List_Delete(List_Struct* head, int num);

#endif