#ifndef _TINY_UI_H
#define _TINY_UI_H

#ifdef TINYUI_GTK
#include <gtk/gtk.h>
#endif
#include <string>
#include <sstream>
#include <iomanip>

#ifdef TINYUI_QT
#include <QObject>

class QWidget;
class QPushButton;
class QBoxLayout;
class QWidget;
class QSocketNotifier;
class QListWidgetItem;
class QListWidget;
#else
#define Q_OBJECT
class QObject {};
#endif

#define UNUSED(x) (void)(x)

#define DISABLE_ASSIGN(x) \
    private: \
        void operator =(const x &from);

#undef foreach
#define foreach(type, i, container) \
    for (type::iterator i = (container).begin(); i != (container).end(); ++i)

#define foreach_safe(type, i, container) \
    for (type::iterator next = (container).begin(), i = next++; \
         i != (container).end(); i = next++)

template<class T>
std::string format_number(T val)
{
    std::ostringstream str;
    str << val;
    return str.str();
}

template<class T>
std::string format_hex(T val, size_t width)
{
    std::ostringstream str;
    str << std::setw(width) << std::setfill('0') << std::hex << val;
    return str.str();
}

enum Orientation {
    VERTICAL,
    HORIZONTAL,
};

class Widget {
public:
    Widget() {}

    void show();
    void hide();

#ifdef TINYUI_GTK
    virtual bool expandable(Orientation orientation) = 0;
    virtual GtkWidget *gtk_widget() = 0;
#endif
#ifdef TINYUI_QT
    virtual QWidget *qt_widget() = 0;
#endif
};

class Button;

class ClickInterface {
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
    Orientation m_orientation;
    bool m_expandable[2];
    GtkWidget *m_gtkwidget;
    bool expandable(Orientation orientation);
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
    void set_handler(ClickInterface *events);

private:
    ClickInterface *m_handler;

#ifdef TINYUI_GTK
    GtkWidget *m_gtkwidget;
    bool expandable(Orientation orientation);
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

class ListBoxItem {
    DISABLE_ASSIGN(ListBoxItem)
    friend class ListBox;
public:
    ListBoxItem(const std::string &text = std::string());
    ~ListBoxItem();

    void set_text(const std::string &text);

private:
#ifdef TINYUI_GTK
    std::string m_text;
    GtkTreeRowReference *m_rowref;
#endif
#ifdef TINYUI_QT
    QListWidgetItem *m_qtitem;
#endif
};

class ListBox;

class ListBoxInterface {
public:
    virtual void clicked(ListBox *listbox, ListBoxItem *item) = 0;
};

class ListBox: public QObject, public Widget {
    DISABLE_ASSIGN(ListBox)
    Q_OBJECT

public:
    ListBox();
    ~ListBox();

    void set_handler(ListBoxInterface *handler);

    void add_item(ListBoxItem *item);
    void scroll_to(ListBoxItem *item);

private:
    ListBoxInterface *m_handler;

#ifdef TINYUI_GTK
    GtkWidget *m_gtkwidget, *m_treeview;
    GtkListStore *m_store;
    bool expandable(Orientation orientation);
    GtkWidget *gtk_widget();
    static void activated_cb(GtkTreeView *treeview, GtkTreePath *path,
                             GtkTreeViewColumn *col, ListBox *listbox);
#endif
#ifdef TINYUI_QT
    QListWidget *m_qtwidget;
    QWidget *qt_widget();
private slots:
    void itemActivated_slot(QListWidgetItem *qtitem);
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

class IoWatchInterface {
public:
    virtual void ready(IoWatch *iowatch) = 0;
};

class IoWatch: public QObject {
    DISABLE_ASSIGN(IoWatch)
    Q_OBJECT

public:
    explicit IoWatch(int fd);
    ~IoWatch();

    void set_handler(IoWatchInterface *events);

private:
    IoWatchInterface *m_handler;

#ifdef TINYUI_GTK
    GIOChannel *m_iochannel;
    int m_id;
    static bool io_watch_cb(GIOChannel *iochannel, GIOCondition cond,
                            IoWatch *iowatch);
#endif
#ifdef TINYUI_QT
    QSocketNotifier *m_notifier;
private slots:
    void activated_slot();
#endif
};

class Timer;

class TimerInterface {
public:
    virtual void timeout(Timer *timer) = 0;
};

class Timer: public QObject {
public:
    explicit Timer(int interval);
    ~Timer();

    void set_handler(TimerInterface *events);

private:
    TimerInterface *m_handler;

#ifdef TINYUI_GTK
    guint m_id;
    static bool timer_cb(Timer *timer);
#endif
#ifdef TINYUI_QT
    int m_id;
    void timerEvent(QTimerEvent *event);
#endif
};

class QuitInterface
{
public:
    virtual void quit() = 0;
};

class SigIntHandler: private IoWatchInterface {
public:
    SigIntHandler();

    void set_handler(QuitInterface *handler);

private:
    QuitInterface *m_handler;
    int m_fd[2];

    void ready(IoWatch *iowatch);

    static SigIntHandler *m_instance;
    static void signal_handler(int sig);
};

class Application: public QuitInterface {
public:
    Application(int *argc, char ***argv);

    void quit();
    int run();

    static Application *instance();

private:
    static Application *m_instance;
};

#endif
