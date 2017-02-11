#include "draw.h"

const char EXPANDING_LETTERS[EXPANDING_LETTERS_NUMBER] = {'j', 'p', 'y', 'q'}; // These letters contain 'tail'
const char HIGH_LETTERS[HIGH_LETTERS_NUMBER] = {'b', 'd', 'f', 'h', 'k', 'l', 't'}; // These letters are small but still require space of big letters.

const double FONT_ARITHMETIC_PLACER = 1.6; // Strictly related to font type to make sure fractal '-' is on the right position, 1.75 for SEGOE UI
const double FRACTAL_PERCENTAGE = 0.6; // Numerator and denumerator scale.
const double POWER_PERCENTAGE = 0.5; // Power and Index scale.

bool contains_expanding_letter(char* text)
{
    for (int i = 0; text[i] != NULL; i++)
        for (int j = 0; j < EXPANDING_LETTERS_NUMBER; j++)
            if (text[i] == EXPANDING_LETTERS[j])
                return true;

    return false;
}

bool contains_high_letter_or_number(char* text)
{
    for (int i = 0; text[i] != NULL; i++)
        if ((text[i] >= 'A' && text[i] <= 'Z') || (text[i] >= '0' && text[i] <= '9'))
            return true;
        else
            for (int j = 0; j < HIGH_LETTERS_NUMBER; j++)
                if (text[i] == HIGH_LETTERS[j])
                    return true;

    return false;
}

// Returns the size difference between expanding and normal small letter.
double size_difference(cairo_t* cr, double scale)
{
    cairo_save(cr);
    cairo_set_font_size(cr, FontSize * scale);
    cairo_text_extents_t extents;
    cairo_text_extents(cr, "x", &extents);
    double small = extents.height;
    cairo_text_extents(cr, "y", &extents);
    cairo_restore(cr);

    return extents.height - small;
}

double small_letter_size(cairo_t* cr, double scale)
{
    cairo_text_extents_t extents;
    cairo_save(cr);
    cairo_set_font_size(cr, FontSize * scale);
    cairo_text_extents(cr, "x", &extents);
    cairo_restore(cr);

    return extents.height;
}

double big_letter_size(cairo_t* cr, double scale)
{
    cairo_text_extents_t extents;
    cairo_save(cr);
    cairo_set_font_size(cr, FontSize * scale);
    cairo_text_extents(cr, "X", &extents);
    cairo_restore(cr);

    return extents.height;
}

//"Twierdzenie 1: Minus unarny mo¿e byæ tylko i wy³¹cznie na pocz¹tku wyra¿eñ:
// (1) "+" z lewej zawsze bez, z prawej zawsze z
// (2) "-" z lewej zawsze bez, z prawej zawsze z
// (3) "*" z lewej zawsze bez, z prawej zawsze z
// (4) "/" licznik bez, mianownik bez
// (5) "^" z lewej zawsze z, z prawej zawsze bez
// (6) "indeks", z lewej zawsze z, z prawej zawsze bez

// The following function calculates the size of each expression.
void calculate_boxes(cairo_t* cr, TreePtr partialTree, double scale)
{
    cairo_set_font_size(cr, FontSize * scale);

    if (partialTree->left == NULL)
    {
        // Case: number or variable
        // So we just set the width, height and negative_height using text_extents.
        cairo_text_extents_t extents;

        cairo_text_extents(cr, partialTree->variable, &extents);
        partialTree->box.width = extents.x_advance;

        if (contains_high_letter_or_number(partialTree->variable))
            partialTree->box.height = big_letter_size(cr, scale);
        else
            partialTree->box.height = gtk_toggle_button_get_active(chkSaveSpace) ? small_letter_size(cr, scale) : big_letter_size(cr, scale); // meaning x/y is drawn differently.

        if (contains_expanding_letter(partialTree->variable))
            partialTree->box.negative_height = size_difference(cr, scale);
        else
            partialTree->box.negative_height = 0;

        return;
    }

    Operator op = partialTree->op;

    if (op.ch == '$')
    {
        // Case: unary minus
        // Let's check what the left operator is (left is the only operator here!)
        // Parenthise if and only if it's the operator and it's not division or power (-1/2 NOT: -(1/2))
        Operator opArg = GetOperator(partialTree->left->op.ch);
        bool bShouldPackInParenthesis = IsOperator(partialTree->left->op.ch) && opArg.ch != '/' && opArg.ch != '^';
        calculate_boxes(cr, partialTree->left, scale);

        cairo_text_extents_t extents;
        cairo_text_extents(cr, "-", &extents);

        double width = 0;
        width += extents.x_advance;

        if (bShouldPackInParenthesis)
        {
            cairo_text_extents(cr, "(", &extents);
            width += extents.x_advance;
            cairo_text_extents(cr, ")", &extents);
            width += extents.x_advance;
        }

        partialTree->box.width = partialTree->left->box.width + width; // The width of minus and box.
        partialTree->box.height = partialTree->left->box.height; // Minus doesn't expand height
        partialTree->box.negative_height = partialTree->left->box.negative_height; // As well as negative height.
    }
    else if (op.ch == '+' || op.ch == '-' || op.ch == '*')
    {
        // Case: Plus, Minus, Multiplication
        // We also add the right parenthesis in case it's unary minus there!
        bool leftParenthesis = is_left_parenthesis_needed(op, partialTree->left->op);
        bool rightParenthesis = is_right_parenthesis_needed(op, partialTree->right->op) || partialTree->right->op.ch == '$';

        calculate_boxes(cr, partialTree->left, scale);
        calculate_boxes(cr, partialTree->right, scale);

        double width = 0;

        cairo_text_extents_t extents;
        if (op.ch == '*')
        {
            cairo_text_extents(cr, " · ", &extents);
        }
        else
        {
            char strOp[5] = " + ";
            strOp[1] = op.ch;
            cairo_text_extents(cr, strOp, &extents);
        }

        width += extents.x_advance;

        if (leftParenthesis)
        {
            cairo_text_extents(cr, "(", &extents);
            width += extents.x_advance;
            cairo_text_extents(cr, ")", &extents);
            width += extents.x_advance;
        }

        if (rightParenthesis)
        {
            cairo_text_extents(cr, "(", &extents);
            width += extents.x_advance;
            cairo_text_extents(cr, ")", &extents);
            width += extents.x_advance;
        }

        // Width is the sum, height and negative height are the maximum of left expression and right expression.
        partialTree->box.width = partialTree->left->box.width + partialTree->right->box.width + width;
        partialTree->box.height = MAX(partialTree->left->box.height, partialTree->right->box.height);
        partialTree->box.negative_height = MAX(partialTree->left->box.negative_height, partialTree->right->box.negative_height);
    }
    else if (op.ch == '/')
    {
        // Case: Division, obviously no parenthesis here.
        calculate_boxes(cr, partialTree->left, scale * FRACTAL_PERCENTAGE);
        cairo_set_font_size(cr, FontSize * scale);
        calculate_boxes(cr, partialTree->right, scale * FRACTAL_PERCENTAGE);
        cairo_set_font_size(cr, FontSize * scale);

        // Obviously, the width of fractal is the maximum of numerator and denumerator.
        partialTree->box.width = MAX(partialTree->left->box.width, partialTree->right->box.width);

        // This is a tricky part here.
        // spacing: is the the size of line together with space between numerator/denumerator and middle line.
        // thatSpace: this is specific to font size, space that is between baseline and the middle line (= +, - base)
        double spacing = LineWidth * scale + size_difference(cr, scale) / 2.0;
        double thatSpace = small_letter_size(cr, scale) / FONT_ARITHMETIC_PLACER;
        partialTree->box.height = partialTree->left->box.height + partialTree->left->box.negative_height + spacing + thatSpace;
        partialTree->box.negative_height = partialTree->right->box.height + partialTree->right->box.negative_height + spacing - thatSpace;
    }
    else if (op.ch == '^')
    {
        // Obviously we don't put the right parenthesis, but we are required to put the left one!
        bool leftParenthesis = is_left_parenthesis_needed(op, partialTree->left->op) || partialTree->left->op.ch == '$';

        double width = 0;
        cairo_text_extents_t extents;
        if (leftParenthesis)
        {
            cairo_text_extents(cr, "(", &extents);
            width += extents.x_advance;
            cairo_text_extents(cr, ")", &extents);
            width += extents.x_advance;
        }

        calculate_boxes(cr, partialTree->left, scale);
        calculate_boxes(cr, partialTree->right, scale * POWER_PERCENTAGE);
        cairo_set_font_size(cr, FontSize * scale);

        partialTree->box.width = partialTree->left->box.width + partialTree->right->box.width + width;
        partialTree->box.height = partialTree->left->box.height + partialTree->right->box.height +
                                    partialTree->right->box.negative_height - partialTree->left->box.height / 4.0;
        partialTree->box.negative_height = partialTree->left->box.negative_height;
    }
    else if (op.ch == '_')
    {
        // Similar to power.
        calculate_boxes(cr, partialTree->left, scale);
        calculate_boxes(cr, partialTree->right, scale * POWER_PERCENTAGE);
        cairo_set_font_size(cr, FontSize * scale);

        partialTree->box.width = partialTree->left->box.width + partialTree->right->box.width;
        partialTree->box.height = partialTree->left->box.height;

        double newNegativ = partialTree->right->box.height + partialTree->right->box.negative_height
                                                                    - small_letter_size(cr, scale) / 4.0;
        partialTree->box.negative_height = MAX(partialTree->left->box.negative_height, newNegativ);
    }
}

// The following function places the expressions in the correct place using the calculated size by calculate_box(...)
// This is important to notice that this function is MIRROR of calculate_box, which means that it must be complementary
// when drawing and calculating positions.
void draw_expression(cairo_t* cr, TreePtr partialTree, double scale, double startX, double startY)
{
    cairo_font_extents_t fe;
    cairo_font_extents(cr, &fe);

    cairo_move_to(cr, startX, startY);
    cairo_set_font_size(cr, FontSize * scale);

    if (partialTree->left == NULL) // Variable or number.
    {
        cairo_text_extents_t extents;
        cairo_text_extents(cr, partialTree->variable, &extents);

        cairo_move_to(cr, startX, startY);
        cairo_show_text(cr, partialTree->variable);
        cairo_move_to(cr, startX, startY);

        return;
    }

    Operator op = partialTree->op;

    if (op.ch == '$')
    {
        // The parenthesis is the exception here.
        Operator opArg = GetOperator(partialTree->left->op.ch);
        bool bShouldPackInParenthesis = IsOperator(partialTree->left->op.ch) && opArg.ch != '/' && opArg.ch != '^';

        cairo_text_extents_t extents;
        cairo_text_extents(cr, "-", &extents);
        cairo_show_text(cr, "-");

        startX += extents.x_advance;

        cairo_move_to(cr, startX, startY);

        if (bShouldPackInParenthesis)
        {
            cairo_text_extents(cr, "(", &extents);
            cairo_show_text(cr, "(");
            startX += extents.x_advance;
        }

        draw_expression(cr, partialTree->left, scale, startX, startY);
        cairo_set_font_size(cr, FontSize * scale);

        startX += partialTree->left->box.width;
        cairo_move_to(cr, startX, startY);

        if (bShouldPackInParenthesis)
        {
            cairo_text_extents(cr, ")", &extents);
            cairo_show_text(cr, ")");
        }
    }
    else if (op.ch == '+' || op.ch == '-' || op.ch == '*')
    {
        bool leftParenthesis = is_left_parenthesis_needed(op, partialTree->left->op);
        bool rightParenthesis = is_right_parenthesis_needed(op, partialTree->right->op) || partialTree->right->op.ch == '$';

        cairo_text_extents_t extents;

        if (leftParenthesis)
        {
            cairo_text_extents(cr, "(", &extents);
            cairo_move_to(cr, startX, startY);
            cairo_show_text(cr, "(");
            cairo_set_font_size(cr, FontSize * scale);

            startX += extents.x_advance;
        }

        draw_expression(cr, partialTree->left, scale, startX, startY);
        cairo_set_font_size(cr, FontSize * scale);

        startX += partialTree->left->box.width;

        if (leftParenthesis)
        {
            cairo_text_extents(cr, ")", &extents);
            cairo_move_to(cr, startX, startY);
            cairo_show_text(cr, ")");

            startX += extents.x_advance;
        }

        cairo_move_to(cr, startX, startY);
        if (op.ch == '*')
        {
            cairo_text_extents(cr, " · ", &extents);
            cairo_move_to(cr, startX, startY);
            cairo_show_text(cr, " · ");
        }
        else
        {
            char strOp[5] = " + ";
            strOp[1] = op.ch;
            cairo_text_extents(cr, strOp, &extents);
            cairo_move_to(cr, startX, startY);
            cairo_show_text(cr, strOp);
        }

        startX += extents.x_advance;
        cairo_move_to(cr, startX, startY);

        if (rightParenthesis)
        {
            cairo_text_extents(cr, "(", &extents);
            cairo_move_to(cr, startX, startY);
            cairo_show_text(cr, "(");

            startX += extents.x_advance;
        }

        draw_expression(cr, partialTree->right, scale, startX, startY);
        cairo_set_font_size(cr, FontSize * scale);

        startX += partialTree->right->box.width;

        if (rightParenthesis)
        {
            cairo_text_extents(cr, ")", &extents);
            cairo_move_to(cr, startX, startY);
            cairo_show_text(cr, ")");

            startX += extents.x_advance;
        }
    }
    else if (op.ch == '/')
    {
        // We calculate the middle for numerator and denumerator.
        double centerNumerator = (partialTree->box.width - partialTree->left->box.width) / 2.0;
        double centerDenumerator = (partialTree->box.width - partialTree->right->box.width) / 2.0;

        double saveY = startY;

        // First we move high enough to draw properly numerator
        double spacing = LineWidth * scale + size_difference(cr, scale) / 2.0;
        double thatSpace = small_letter_size(cr, scale) / FONT_ARITHMETIC_PLACER;
        startY -= thatSpace;
        startY -= spacing;
        startY -= partialTree->left->box.negative_height;

        draw_expression(cr, partialTree->left, scale * FRACTAL_PERCENTAGE, startX + centerNumerator, startY);
        cairo_set_font_size(cr, FontSize * scale);

        startY += partialTree->left->box.negative_height;
        startY += spacing;

        // Then we move back to draw properly middle line
        cairo_move_to(cr, startX, startY);
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_set_line_width(cr, LineWidth * scale);
        cairo_line_to(cr, startX + partialTree->box.width, startY);
        cairo_stroke(cr);

        startY = saveY;

        startY += partialTree->box.negative_height;
        startY -= partialTree->right->box.negative_height;

        // Then we finish by drawing denumerator.
        draw_expression(cr, partialTree->right, scale * FRACTAL_PERCENTAGE, startX + centerDenumerator, startY);
    }
    else if (op.ch == '^')
    {
        bool leftParenthesis = is_left_parenthesis_needed(op, partialTree->left->op) || partialTree->left->op.ch == '$';

        cairo_text_extents_t extents;

        if (leftParenthesis)
        {
            cairo_text_extents(cr, "(", &extents);
            cairo_move_to(cr, startX, startY);
            cairo_show_text(cr, "(");

            startX += extents.x_advance;

            cairo_set_font_size(cr, FontSize * scale);
        }

        draw_expression(cr, partialTree->left, scale, startX, startY);
        cairo_set_font_size(cr, FontSize * scale);

        startX += partialTree->left->box.width;
        if (leftParenthesis)
        {
            cairo_text_extents(cr, ")", &extents);
            cairo_move_to(cr, startX, startY);
            cairo_show_text(cr, ")");

            startX += extents.x_advance;
        }

        // We set the Y the way to draw the power at the height of 1/4 from the top of base.
        startY -= partialTree->left->box.height;
        startY -= partialTree->right->box.negative_height;
        startY += partialTree->left->box.height / 4.0;

        draw_expression(cr, partialTree->right, scale * POWER_PERCENTAGE, startX, startY);
    }
    else if (op.ch == '_')
    {
        // Very similar
        draw_expression(cr, partialTree->left, scale, startX, startY);

        startX += partialTree->left->box.width;

        startY += partialTree->right->box.height;
        startY -= small_letter_size(cr, scale) / 4.0;

        draw_expression(cr, partialTree->right, scale * POWER_PERCENTAGE, startX, startY);
    }
}
