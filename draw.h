// Copyright (C) Jakub Kuczkowiak 2017

#pragma once

#include <gtk/gtk.h>
#include <stdbool.h>
#include "tree.h"

#define EXPANDING_LETTERS_NUMBER 4
#define HIGH_LETTERS_NUMBER 7

// Global variables that keep the instance of CheckButton, FontSize and LineWidth (related to FontSize)
extern GtkWidget *chkSaveSpace;

extern double FontSize;
extern double LineWidth;

bool contains_expanding_letter(char* text);
bool contains_high_letter_or_number(char* text);

double size_difference(cairo_t* cr, double scale);
double small_letter_size(cairo_t* cr, double scale);
double big_letter_size(cairo_t* cr, double scale);

void calculate_boxes(cairo_t* cr, TreePtr partialTree, double scale);
void draw_expression(cairo_t* cr, TreePtr partialTree, double scale, double startX, double startY);
