// Jakub Kuczkowiak
// Biblioteka prezentowana na wykladzie "Wstep do C"
// Autor: Marek Piotrów

#include <stdio.h>
#include "stack.h"
#include <stdlib.h>

struct stack
{
    TYP_INFO info;
    struct stack *next;
};

void init(StackPtr *stck)
{
    *stck = NULL;
}

bool isempty(StackPtr stck)
{
    return (stck == NULL);
}

bool push(StackPtr *stck, TYP_INFO info)
{
    StackPtr p;
    if ((p = (StackPtr)malloc(sizeof(struct stack))) == NULL)
        return true;
    else
    {
        p->info=info;
        p->next=*stck;
        *stck=p;
        return false;
    }
}

TYP_INFO pop(StackPtr *stck)
{
    TYP_INFO info;
    StackPtr p;
    if (*stck == NULL)
        return 0;
    else
    {
        info=(*stck)->info;
        p=*stck;
        *stck=(*stck)->next;
        free(p);
        return info;
    }
}
