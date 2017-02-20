// Copyright (C) Jakub Kuczkowiak 2017

#include "tex.h"
#include "draw.h"

static GtkWidget *txtInput;
static GtkWidget *window;
static GtkWidget *drawArea;
GtkWidget *chkSaveSpace;
static GtkWidget *slider;

double FontSize;
double LineWidth; // it gets initalized in the very beginning: FONT_SIZE_DEFAULT / 50.0;
const double FONT_SIZE_DEFAULT = 120; // 120; // >= 266 IS PRETTY!

int expression_count = 0;
const int MAX_EXPRESSION_COUNT = 666;
TreePtr expressions[666];

// This one shows error dialog with custom message.
void show_messagebox_error(const gchar* message)
{
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    GtkWidget* dialog = gtk_message_dialog_new (GTK_WINDOW(window),
                                         flags,
                                         GTK_MESSAGE_ERROR,
                                         GTK_BUTTONS_CLOSE,
                                         message);
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
}

static void btnCompute(GtkWidget *widget, gpointer data)
{
    const char* myText = gtk_entry_get_text(GTK_ENTRY(txtInput));

    char clear_expression[MAX_EXPRESSION_LENGTH];
    clear_expression_from_spaces(myText, clear_expression);
    if (clear_expression[0] == '\0')
    {
        show_messagebox_error("Empty expression!");
        return;
    }

    printf("\nExpression without spaces: %s\n", clear_expression);

    if (!is_correct_input(clear_expression))
    {
        show_messagebox_error("Incorrect characters inside input!");
        return;
    }

    char expression[MAX_EXPRESSION_LENGTH];
    encode_unary_minuses(clear_expression, expression);

    printf("Expression with unary - encoded: %s\n", expression);

    char result[MAX_EXPRESSION_LENGTH];
    gchar error[64] = "";
    if (change_to_RPN(expression, result, error))
    {
        TreePtr korzen;
        if (!create_tree(result, &korzen))
        {
            show_messagebox_error("Wrong expression syntax");
            return;
        }

        expressions[expression_count++] = korzen;
        gtk_widget_queue_draw(drawArea);
    }
    else
    {
        show_messagebox_error(error);
    }
}

static void do_drawing(cairo_t *cr);

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr,
	gpointer user_data)
{
	do_drawing(cr);

	return FALSE;
}

static void do_drawing(cairo_t* cr)
{
    cairo_select_font_face(cr, "Arial",
            CAIRO_FONT_SLANT_NORMAL,
            CAIRO_FONT_WEIGHT_NORMAL);

    if (expression_count == 0) return;

    double defaultDelimeter = FontSize / 2;
    double startX = defaultDelimeter, startY = defaultDelimeter;
    double width = 0, height = 0;

    for (int i = 0; i < expression_count; i++)
    {
        height += expressions[i]->box.height + expressions[i]->box.negative_height + defaultDelimeter;
        width = MAX(width, expressions[i]->box.width);
    }

    width += startX * 2;
    if (drawArea != NULL)
        gtk_widget_set_size_request(drawArea, (gint)width, (gint)(height + defaultDelimeter));

    for (int i = 0; i < expression_count; i++)
    {
        startY += expressions[i]->box.height;
        calculate_boxes(cr, expressions[i], 1, true);
        draw_expression(cr, expressions[i], 1, startX, startY, true);

        startY += expressions[i]->box.negative_height + defaultDelimeter;
    }

	cairo_stroke(cr);
}

void chkSaveSpace_clicked()
{
    gtk_widget_queue_draw(drawArea);
}

void slider_value_changed()
{
    FontSize = gtk_range_get_value(GTK_RANGE(slider));
    LineWidth = FontSize / 50.0;
    gtk_widget_queue_draw(drawArea);
}

void initialize()
{
    FontSize = FONT_SIZE_DEFAULT;
    LineWidth = FontSize / 50.0;
}

void setup_ui(int argc, char *argv[])
{
    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Mathematical Expression Project");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_maximize(GTK_WINDOW(window));
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

    txtInput = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(txtInput), MAX_EXPRESSION_LENGTH);
    gtk_entry_set_text(GTK_ENTRY(txtInput), "Type in expression...");
    gtk_editable_select_region(GTK_EDITABLE(txtInput), 0, gtk_entry_get_text_length(GTK_ENTRY(txtInput)));

    chkSaveSpace = gtk_check_button_new_with_label("Save space");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(chkSaveSpace), true);
    g_signal_connect(G_OBJECT(chkSaveSpace), "toggled", G_CALLBACK(chkSaveSpace_clicked), NULL);

    GtkWidget* lblFontSize = gtk_label_new("Font size: ");
    slider = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 32, 400, 1);
    gtk_range_set_value(GTK_RANGE(slider), FONT_SIZE_DEFAULT);
    gtk_scale_set_value_pos(GTK_SCALE(slider), GTK_POS_LEFT);
    g_signal_connect(G_OBJECT(slider), "value_changed", G_CALLBACK(slider_value_changed), NULL);

    GtkWidget* font_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(font_box), lblFontSize, false, true, 0);
    gtk_box_pack_start(GTK_BOX(font_box), slider, true, true, 0);

    GtkWidget *button = gtk_button_new_with_label("Compute");
    g_signal_connect(G_OBJECT(button), "clicked",
                     G_CALLBACK(btnCompute),(gpointer)"Przycisk");

    GtkWidget* v_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(v_box), txtInput, false, true, 0);
    gtk_box_pack_start(GTK_BOX(v_box), chkSaveSpace, false, true, 0);
    gtk_box_pack_start(GTK_BOX(v_box), font_box, false, false, 0);
    gtk_box_pack_start(GTK_BOX(v_box), button, false, true, 0);

    drawArea = gtk_drawing_area_new();
    g_signal_connect(G_OBJECT(drawArea), "draw", G_CALLBACK(on_draw_event), NULL);

    GtkWidget* swindow = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(swindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(v_box), swindow, true, true, 0);

    GtkWidget* label = gtk_label_new("Copyright (C) Jakub Kuczkowiak 2017");
    gtk_box_pack_start(GTK_BOX(v_box), label, false, true, 0);

    gtk_container_add(GTK_CONTAINER(window), v_box);

    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(swindow), drawArea);

    gtk_widget_show_all(window);
    gtk_main();
}

int main(int argc, char *argv[])
{
    initialize();
    setup_ui(argc, argv);

    return 0;
}
