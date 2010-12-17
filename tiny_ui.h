#ifndef _TINY_UI_H
#define _TINY_UI_H

#ifdef TINYUI_GTK
#include <gtk/gtk.h>
#endif
#include <string>
#include <sstream>

#ifdef TINYUI_QT
#include <QObject>

class QWidget;
class QPushButton;
class QBoxLayout;
class QWidget;
class QSocketNotifier;
#else
#define Q_OBJECT
class QObject {};
#endif

#define UNUSED(x) (void)(x)

#define DISABLE_ASSIGN(x) \
    private: \
        void operator =(const x &from);

template<class T>
std::string format_number(T val)
{
    std::ostringstream str;
    str << val;
    return str.str();
}

enum Orientation {
    VERTICAL,
    HORIZONTAL,
};

class Widget {
public:
    Widget() {}

#ifdef TINYUI_GTK
    virtual GtkWidget *gtk_widget() = 0;
#endif
#ifdef TINYUI_QT
    virtual QWidget *qt_widget() = 0;
#endif
};

class Button;

class ButtonEvents {
public:
    virtual void clicked(Button *button) = 0;
};

class BoxLayout: public Widget {
    DISABLE_ASSIGN(BoxLayout)
public:
    BoxLayout(Orientation orientation);
    ~BoxLayout();

    void add_widget(Widget *widget);

private:
#ifdef TINYUI_GTK
    GtkWidget *m_gtkwidget;
    GtkWidget *gtk_widget();
#endif
#ifdef TINYUI_QT
    QBoxLayout *m_qtlayout;
    QWidget *m_qtwidget;
    QWidget *qt_widget();
#endif
};

class Button: public QObject, public Widget {
    DISABLE_ASSIGN(Button)
    Q_OBJECT

public:
    explicit Button(const std::string &label = std::string());
    ~Button();

    void set_label(const std::string &label);
    void set_events(ButtonEvents *events);

private:
    ButtonEvents *m_events;

#ifdef TINYUI_GTK
    GtkWidget *m_gtkwidget;
    GtkWidget *gtk_widget();
    static void clicked_cb(GtkWidget *widget, Button *button);
#endif
#ifdef TINYUI_QT
    QPushButton *m_qtwidget;
    QWidget *qt_widget();
private slots:
    void clicked_slot();
#endif
};

class Window {
    DISABLE_ASSIGN(Window)
public:
    explicit Window(const std::string &title);
    ~Window();
    void set_widget(Widget *widget);
    void show();

private:
#ifdef TINYUI_GTK
    GtkWidget *m_gtkwidget;
#endif
#ifdef TINYUI_QT
    QWidget *m_qtwidget;
    std::string m_title;
#endif
};

class IoWatch;

class IoWatchEvents {
public:
    virtual void ready(IoWatch *iowatch) = 0;
};

class IoWatch: public QObject {
    DISABLE_ASSIGN(IoWatch)
    Q_OBJECT

public:
    explicit IoWatch(int fd);
    ~IoWatch();

    void set_events(IoWatchEvents *events);

private:
    IoWatchEvents *m_events;

#ifdef TINYUI_GTK
    GIOChannel *m_iochannel;
    static bool io_watch_cb(GIOChannel *iochannel, GIOCondition cond,
                            IoWatch *iowatch);
#endif
#ifdef TINYUI_QT
    QSocketNotifier *m_notifier;
private slots:
    void activated_slot();
#endif
};

class Application {
public:
    Application(int *argc, char ***argv);

    void quit();
    int run();

    static Application *instance();

private:
    static Application *m_instance;
};

#endif
