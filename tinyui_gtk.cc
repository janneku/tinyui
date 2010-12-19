/*
 * gtk+ 2 implementation of tinyui GUI classes
 */
#include "tiny_ui.h"
#include <string.h>
#include <stdexcept>

BoxLayout::BoxLayout(Orientation orientation)
{
    switch (orientation) {
    case HORIZONTAL:
        m_gtkwidget = gtk_hbox_new(false, 3);
        break;
    case VERTICAL:
        m_gtkwidget = gtk_vbox_new(false, 3);
        break;
    default:
        throw std::runtime_error("Invalid orientation");
    }
}

BoxLayout::~BoxLayout()
{
    gtk_widget_destroy(m_gtkwidget);
}

void BoxLayout::add_widget(Widget *widget)
{
    gtk_box_pack_start(GTK_BOX(m_gtkwidget), widget->gtk_widget(),
                       true, true, 0);
}

GtkWidget *BoxLayout::gtk_widget()
{
    return m_gtkwidget;
}

Button::Button(const std::string &label) :
    m_handler(NULL)
{
    m_gtkwidget = gtk_button_new_with_label(label.c_str());
    g_signal_connect(m_gtkwidget, "clicked", G_CALLBACK(clicked_cb), this);
}

Button::~Button()
{
    gtk_widget_destroy(m_gtkwidget);
}

void Button::set_label(const std::string &label)
{
    gtk_button_set_label(GTK_BUTTON(m_gtkwidget), label.c_str());
}

GtkWidget *Button::gtk_widget()
{
    return m_gtkwidget;
}

void Button::clicked_cb(GtkWidget *widget, Button *button)
{
    UNUSED(widget);
    if (button->m_handler)
        button->m_handler->clicked(button);
}

ListBoxItem::ListBoxItem(const std::string &text) :
    m_text(text), m_rowref(NULL)
{
}

ListBoxItem::~ListBoxItem()
{
    if (m_rowref == NULL)
        return;

    GtkTreeModel *model = gtk_tree_row_reference_get_model(m_rowref);
    GtkTreePath *path = gtk_tree_row_reference_get_path(m_rowref);
    GtkTreeIter iter;
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_path_free(path);

    gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
    gtk_tree_row_reference_free(m_rowref);
}

void ListBoxItem::set_text(const std::string &text)
{
    m_text = text;
    if (m_rowref == NULL)
        return;

    GtkTreeModel *model = gtk_tree_row_reference_get_model(m_rowref);
    GtkTreePath *path = gtk_tree_row_reference_get_path(m_rowref);
    GtkTreeIter iter;
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_path_free(path);

    gtk_list_store_set(GTK_LIST_STORE(model), &iter, 0, text.c_str(), -1);
}

ListBox::ListBox() :
    m_handler(NULL)
{
    m_store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);
    m_treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(m_store));
    g_signal_connect(m_treeview, "row-activated", G_CALLBACK(activated_cb), this);
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(m_treeview), false);

    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(m_treeview),
        -1, "", renderer, "text", 0, NULL);

    m_gtkwidget = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(m_gtkwidget), m_treeview);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(m_gtkwidget),
            GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
}

ListBox::~ListBox()
{
    gtk_widget_destroy(m_gtkwidget);
}

void ListBox::add_item(ListBoxItem *item)
{
    GtkTreeIter iter;
    gtk_list_store_append(m_store, &iter);
    gtk_list_store_set(m_store, &iter, 0, item->m_text.c_str(), 1, item, -1);

    GtkTreePath *path =
        gtk_tree_model_get_path(GTK_TREE_MODEL(m_store), &iter);
    item->m_rowref = gtk_tree_row_reference_new(GTK_TREE_MODEL(m_store), path);
    gtk_tree_path_free(path);
}

void ListBox::scroll_to(ListBoxItem *item)
{
    GtkTreePath *path = gtk_tree_row_reference_get_path(item->m_rowref);
    gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(m_treeview), path, NULL,
                                 false, 0, 0);
    gtk_tree_path_free(path);
}

void ListBox::activated_cb(GtkTreeView *treeview, GtkTreePath *path,
                           GtkTreeViewColumn *col, ListBox *listbox)
{
    UNUSED(treeview);
    UNUSED(path);
    UNUSED(col);
    GtkTreeIter iter;
    gtk_tree_model_get_iter(GTK_TREE_MODEL(listbox->m_store), &iter, path);

    GValue val;
    memset(&val, 0, sizeof val);
    gtk_tree_model_get_value(GTK_TREE_MODEL(listbox->m_store), &iter, 1, &val);
    ListBoxItem *item =
        reinterpret_cast<ListBoxItem *>(g_value_get_pointer(&val));
    g_value_unset(&val);

    if (listbox->m_handler)
        listbox->m_handler->clicked(listbox, item);
}

GtkWidget *ListBox::gtk_widget()
{
    return m_gtkwidget;
}

Window::Window(const std::string &title)
{
    m_gtkwidget = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width(GTK_CONTAINER(m_gtkwidget), 5);
    gtk_window_set_title(GTK_WINDOW(m_gtkwidget), title.c_str());
}

Window::~Window()
{
    gtk_widget_destroy(m_gtkwidget);
}

void Window::set_widget(Widget *widget)
{
    gtk_container_add(GTK_CONTAINER(m_gtkwidget), widget->gtk_widget());
}

void Window::show()
{
    gtk_widget_show_all(m_gtkwidget);
}

IoWatch::IoWatch(int fd) :
    m_handler(NULL)
{
    m_iochannel = g_io_channel_unix_new(fd);
    m_id = g_io_add_watch(m_iochannel, G_IO_IN, GIOFunc(io_watch_cb), this);
}

IoWatch::~IoWatch()
{
    g_source_remove(m_id);
    g_io_channel_unref(m_iochannel);
}

bool IoWatch::io_watch_cb(GIOChannel *iochannel, GIOCondition cond,
                          IoWatch *iowatch)
{
    UNUSED(cond);
    UNUSED(iochannel);
    if (iowatch->m_handler)
        iowatch->m_handler->ready(iowatch);
    return true;
}

Timer::Timer(int interval) :
    m_handler(NULL)
{
    m_id = g_timeout_add(interval, GSourceFunc(timer_cb), this);
}

Timer::~Timer()
{
    g_source_remove(m_id);
}

bool Timer::timer_cb(Timer *timer)
{
    if (timer->m_handler)
        timer->m_handler->timeout(timer);
    return true;
}

Application::Application(int *argc, char ***argv)
{
    if (m_instance)
        throw std::runtime_error("Application instance already created");
    m_instance = this;
    gtk_init(argc, argv);
}

void Application::quit()
{
    gtk_main_quit();
}

int Application::run()
{
    gtk_main();
    return 0;
}
