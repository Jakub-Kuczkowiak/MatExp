// Copyright (C) Jakub Kuczkowiak 2017
// This file contains the logical implementation of 'unpacking' expressions.

#pragma once

#include <stdbool.h>
#include <gtk/gtk.h>

#define MAX_EXPRESSION_LENGTH 1000
#define OPERATORS_NUMBER 8

enum Associativity
{
    left,
    right
};

typedef enum Associativity Associativity;

struct Operator
{
    char ch;
    int precedence;
    Associativity associativity;
    bool przemiennosc_nawiasowa;
};

typedef struct Operator Operator;

bool IsOperator(char c);
struct Operator GetOperator(char c);

bool change_to_RPN(char* expression, char* result, gchar* error);

void clear_expression_from_spaces(const char* expression, char* result);
bool is_correct_input(char* expression);
void encode_unary_minuses(char* expression, char* result);
