// Copyright (C) Jakub Kuczkowiak 2017

#include "tex.h"
#include "tree.h"
#include "stack.h"
#include <string.h>

// The following structure contains the definition of operators that we allow to be used.
// The empty operator is used to make the code more readable in some sections.
Operator Operators[OPERATORS_NUMBER] = {
                {'\0', 0},
                {'_', 100},
                {'+', 2, left, false},
                {'-', 2, left, true},
                {'*', 3, left, false},
                {'$', 3, left, false}, // This is so called hidden (-1)* operator! Here the associativity is equal to ANY
                {'/', 3, left, true},
                //{'%', 3},
                {'^', 4, right, true},
                };

// Removes all the spaces.
void clear_expression_from_spaces(const char* expression, char* result)
{
    int len = strlen(expression);
    int j = 0;
    for (int i = 0; i < len; i++)
        if (expression[i] != ' ')
            result[j++] = expression[i];

    result[j] = '\0';
}

// Checks if input contains allowed characters.
bool is_correct_input(char* expression)
{
    int len = strlen(expression);
    for (int i = 0; i < len; i++)
    {
        if (!IsOperator(expression[i]) && (expression[i] < '0' || expression[i] > '9') &&
            (expression[i] < 'a' || expression[i] > 'z') && (expression[i] < 'A' || expression[i] > 'Z')
             && expression[i] != '(' && expression[i] != ')' && expression[i] != '{' && expression[i] != '}'
             && expression[i] != '_' && expression[i] != '.')
            return false;
    }

    return true;
}

// Loop invariant: Unary minus might be first and only first character of each expression and subexpression. (starting from '(')
void encode_unary_minuses(char* expression, char* result)
{
    result[0] = expression[0] == '-' ? '$' : expression[0];

    int len = strlen(expression);

    for (int i = 1; i < len; i++)
        if (expression[i] == '-' && (expression[i - 1] == '(' || expression[i - 1] == '{'))
            result[i] = '$';
        else
            result[i] = expression[i];

    result[len] = '\0';
}

bool IsOperator(char c)
{
    for (int i = 1; i < OPERATORS_NUMBER; i++) // Here we don't want to check an empty operator zero.
        if (c == Operators[i].ch)
            return true;

    return false;
}

struct Operator GetOperator(char c)
{
    for (int i = 0; i < OPERATORS_NUMBER; i++)
    {
        if (c == Operators[i].ch)
            return Operators[i];
    }

    return Operators[0];
}

// The following function is the heart of logic and changes the expression into Reverse Polish Notation.
// It also finds errors with double '.' or mismatching parenthesis.
bool change_to_RPN(char* expression, char* vector, gchar* error)
{
    StackPtr stack;
    init(&stack);

    int vector_iterator = 0;

    char _stack[MAX_EXPRESSION_LENGTH];
    int position = -1;
    int len = strlen(expression);

    for (int i = 0; i < len; i++)
    {
        if (expression[i] == '(')
        {
            _stack[++position] = '(';
        }
        else if (expression[i] == ')') // tutaj wyswietlamy wszystkie dopoki nie napotakmy lewego
        {
            while (_stack[position] != '(')
            {
                if (_stack[position] == '{')
                {
                    sprintf(error, "No opening '('!");
                    return false;
                }

                vector[vector_iterator++] = _stack[position];
                vector[vector_iterator++] = ' ';

                position--;

                if (position == -1)
                {
                    // MISMATCHED PARENTHESIS, BUT CHECk if eXACTLY 0 -1 or something!!!
                    sprintf(error, "No opening '('!");
                    return false;
                }
            }

            // odczytujemy rowniez ten nawias, ale nie wysylamy na wyjscie!
            position--;
        }
        else if (expression[i] == '_')
        {
            //_stack[++position] = '_';
        }
        else if (expression[i] == '{') // indeks operator
        {
            _stack[++position] = '{';
        }
        else if (expression[i] == '}')
        {
            while (_stack[position] != '{')
            {
                if (_stack[position] == '(')
                {
                    // ERROR! MISMATCHING PARENTHESIS!
                    sprintf(error, "No opening '{'!");
                    return false;
                }

                vector[vector_iterator++] = _stack[position];
                vector[vector_iterator++] = ' ';

                position--;

                if (position == -1)
                {
                    // MISMATCHED PARENTHESIS, BUT CHECk if eXACTLY 0 -1 or something!!!
                    sprintf(error, "No opening '{'!");
                    return false;
                }
            }

            // Odczytujemy rowniez ten nawias
            position--;

            vector[vector_iterator++] = '_';
            vector[vector_iterator++] = ' ';

            // sprawdzamy czy poprzedni znak to _
//            if (_stack[position != '_')
//                return false;
//
//            position--;
        }
        //else if (expression[i] == '$')
        //{

        //}
        else if (IsOperator(expression[i]))
        {
            // Difficult condition here.
            Operator current = GetOperator(expression[i]);

            if (position != -1)
            {
                // Not an empty _stack!
                Operator onstackOperator = GetOperator(_stack[position]);

                while ((current.associativity == left && current.precedence <= onstackOperator.precedence)
                       || (current.associativity == right && current.precedence < onstackOperator.precedence))
                {
                    vector[vector_iterator++] = _stack[position];
                    vector[vector_iterator++] = ' ';

                    position--;

                    onstackOperator = GetOperator(_stack[position]);
                    // if (onstackOperator.precedence != 0) break;
                    // TODO: Prove, that this line is not needed!
                }
            }

            _stack[++position] = expression[i];
        }
        else
        {
            int pointCounter = 0; // Counts '.' There can be only one integral and one fraction part.
            do
            {
                vector[vector_iterator++] = expression[i];
                if (expression[i] == '.')
                    if (++pointCounter > 1)
                    {
                        sprintf(error, "Each variable can have only one integral and fraction part (too many '.')!");
                        return false;
                    }

                i++;
            } while (i < len && expression[i] != '(' && expression[i] != ')' && expression[i] != '{'
                     && expression[i] != '}' && !IsOperator(expression[i]));

            i--;
            vector[vector_iterator++] = ' ';
        }
    }

    while (position > -1)
    {
        if (_stack[position] == '(' || _stack[position] == ')' || _stack[position] == '{' || _stack[position] == '}')
        {
            // ERROR: mismatched parenthesis
            sprintf(error, "No enclosing brackets!");
            return false;
        }

        vector[vector_iterator++] = _stack[position];
        vector[vector_iterator++] = ' ';
        position--;
    }

    vector[--vector_iterator] = '\0';
    for (int i = 0; i < vector_iterator; i++)
    {
        printf("%c", vector[i]);
    }

    return true;
}
