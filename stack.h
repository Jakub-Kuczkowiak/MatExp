// Jakub Kuczkowiak
// Biblioteka prezentowana na wykladzie "Wstep do C"
// Autor: Marek Piotrów
// The implemntation of simple stack.

#pragma once

#include <stdbool.h>
#define TYP_INFO void* // Wsk_drzewa
#define TYP_NULL NAN

typedef struct stack *StackPtr;
void init(StackPtr *stck);
bool isempty(StackPtr stck);
bool push(StackPtr *stck, TYP_INFO info);
TYP_INFO pop(StackPtr* stck);
