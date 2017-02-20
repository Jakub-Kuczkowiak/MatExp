// Copyright (C) Jakub Kuczkowiak 2017

#include <stdio.h>
#include "tree.h"
#include "stack.h"
#include <stdlib.h>

#define MAX_VARIABLE_LENGTH 64

extern Operator Operators[OPERATORS_NUMBER];

TreePtr create_leaf(char* variable)
{
    TreePtr leaf;

    if ((leaf = (TreePtr)malloc(sizeof(struct Node))) == NULL) {
        //fprintf(stderr,"No memory for %lf\n",dana);
        exit(0);
    }

    leaf->left = NULL;
    leaf->right = NULL;
    leaf->op = Operators[0];
    leaf->variable = variable;

    return leaf;
}

TreePtr create_node(TreePtr leftLeaf, TreePtr rightLeaf, Operator op)
{
    TreePtr node;

    if ((node = (TreePtr)malloc(sizeof(struct Node))) == NULL) {
        //fprintf(stderr,"No memory for%lf\n",dana);
        exit(0);
    }

    node->left = leftLeaf;
    node->right = rightLeaf;
    node->op = op;

    return node;
}

bool create_tree(const char* vector, TreePtr* korzen)
{
    StackPtr stack;
    init(&stack);

    for (int i = 0; vector[i] != '\0'; i++)
    {
        if (vector[i] == ' ') continue;

        Operator op = GetOperator(vector[i]);

        if (op.ch == '$') // Unary minus
        {
            TreePtr only = pop(&stack);
            if (only == NULL) return false;

            TreePtr wezel = create_node(only, NULL, op);

            push(&stack, wezel);
        }
        else if (op.ch != '\0') // This is operator.
        {
            // We pop two leafs and put one new.
            TreePtr right = pop(&stack);
            TreePtr left = pop(&stack);
            if (left == NULL || right == NULL) return false;

            TreePtr node = create_node(left, right, op);

            push(&stack, node);
        }
        else // Variable / number case
        {
            char* variable = malloc(sizeof(char) * MAX_VARIABLE_LENGTH);
            int j = 0;
            while (vector[i] != '\0' && vector[i] != ' ')
            {
                variable[j] = vector[i];
                i++; j++;
            }

            i--;

            variable[j] = '\0';

            TreePtr leaf = create_leaf(variable);
            push(&stack, leaf);
        }
    }

    TreePtr Korzen = pop(&stack);

    // We check if the stack is empty since there might still be some variables operators on the stack and it's incorrect then.
    if (!isempty(stack)) return false;

    *korzen = Korzen;
    return true;
}

bool is_left_parenthesis_needed(const Operator op, const Operator left)
{
    return IsOperator(left.ch) && (left.precedence < op.precedence ||
            (left.precedence == op.precedence && op.associativity == right && op.przemiennosc_nawiasowa));
}

bool is_right_parenthesis_needed(const Operator op, const Operator right)
{
    return IsOperator(right.ch) && (right.precedence < op.precedence ||
            (right.precedence == op.precedence && op.associativity == left && op.przemiennosc_nawiasowa));
}
