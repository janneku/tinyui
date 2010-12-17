#include "tiny_ui.h"
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
    m_events(NULL)
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
void Button::set_events(ButtonEvents *events)
{
    m_events = events;
}

GtkWidget *Button::gtk_widget()
{
    return m_gtkwidget;
}

void Button::clicked_cb(GtkWidget *widget, Button *button)
{
    UNUSED(widget);
    if (button->m_events)
        button->m_events->clicked(button);
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
    m_events(NULL)
{
    m_iochannel = g_io_channel_unix_new(fd);
    g_io_add_watch(m_iochannel, G_IO_IN, GIOFunc(io_watch_cb), this);
}

IoWatch::~IoWatch()
{
    g_io_channel_unref(m_iochannel);
}

void IoWatch::set_events(IoWatchEvents *events)
{
    m_events = events;
}

bool IoWatch::io_watch_cb(GIOChannel *iochannel, GIOCondition cond,
                          IoWatch *iowatch)
{
    UNUSED(cond);
    UNUSED(iochannel);
    if (iowatch->m_events)
        iowatch->m_events->ready(iowatch);
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

Application *Application::instance()
{
    return m_instance;
}

Application *Application::m_instance;

