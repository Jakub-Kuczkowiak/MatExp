// Copyright (C) Jakub Kuczkowiak 2017

#include <stdio.h>
#include "tree.h"
#include "stack.h"

#define MAX_VARIABLE_LENGTH 64

TreePtr create_leaf(char* variable)
{
    TreePtr leaf;

    if ((leaf = (TreePtr)malloc(sizeof(struct Node))) == NULL) {
        //fprintf(stderr,"No memory for %lf\n",dana);
        exit(0);
    }

    Operator op;
    leaf->left = NULL;
    leaf->right = NULL;
    leaf->op = op;
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

bool create_tree(char* vector, TreePtr* korzen)
{
    StackPtr stack;
    init(&stack);

    for (int i = 0; vector[i] != NULL; i++)
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
            while (vector[i] != NULL && vector[i] != ' ')
            {
                variable[j] = vector[i];
                i++; j++;
            }

            i--;

            variable[j] = NULL;

            TreePtr leaf = create_leaf(variable);
            push(&stack, leaf);
        }
    }

    TreePtr Korzen = pop(&stack);

    *korzen = Korzen;
    return true;
}

bool is_left_parenthesis_needed(Operator op, Operator left)
{
    return (left.precedence < op.precedence ||
            (left.precedence == op.precedence && op.associativity == right && op.przemiennosc_nawiasowa));
}

bool is_right_parenthesis_needed(Operator op, Operator right)
{
    return (right.precedence < op.precedence ||
            (right.precedence == op.precedence && op.associativity == left && op.przemiennosc_nawiasowa));
}
