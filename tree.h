// Copyright (C) Jakub Kuczkowiak 2017
// The following structure creates the tree from ONP.

#pragma once

#include "tex.h"

struct Rectangle
{
    double width;
    double height;
    double negative_height; // This is the rest below base line.
};

typedef struct Node* TreePtr;

struct Node
{
    char* variable;
    Operator op;

    TreePtr left;
    TreePtr right;

    struct Rectangle box;
};

TreePtr create_leaf(char* variable);
TreePtr create_node(TreePtr leftLeaf, TreePtr rightLeaf, Operator op);

bool create_tree(const char* vector, TreePtr* korzen);

bool is_left_parenthesis_needed(const Operator op, const Operator left);
bool is_right_parenthesis_needed(const Operator op, const Operator right);
