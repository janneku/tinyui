/*
 * Qt 4 implementation of tinyui GUI classes
 */
#include "tiny_ui.h"
#include <stdexcept>
#include <QBoxLayout>
#include <QPushButton>
#include <QSocketNotifier>
#include <QListWidget>
#include <QApplication>
#include <QTimerEvent>

void Widget::show()
{
    qt_widget()->show();
}

void Widget::hide()
{
    qt_widget()->hide();
}

BoxLayout::BoxLayout(Orientation orientation)
{
    switch (orientation) {
    case HORIZONTAL:
        m_qtlayout = new QHBoxLayout;
        break;
    case VERTICAL:
        m_qtlayout = new QVBoxLayout;
        break;
    default:
        throw std::runtime_error("Invalid orientation");
    }
    m_qtwidget = new QWidget;
    m_qtwidget->setLayout(m_qtlayout);
}

BoxLayout::~BoxLayout()
{
    delete m_qtwidget;
}

void BoxLayout::add_widget(Widget *widget)
{
    m_qtlayout->addWidget(widget->qt_widget());
}

QWidget *BoxLayout::qt_widget()
{
    return m_qtwidget;
}

Button::Button(const std::string &label) :
    m_handler(NULL)
{
    m_qtwidget = new QPushButton(QString::fromStdString(label));
    connect(m_qtwidget, SIGNAL(clicked()), SLOT(clicked_slot()));
}

Button::~Button()
{
    delete m_qtwidget;
}

void Button::set_label(const std::string &label)
{
    m_qtwidget->setText(QString::fromStdString(label));
}

QWidget *Button::qt_widget()
{
    return m_qtwidget;
}

void Button::clicked_slot()
{
    if (m_handler)
        m_handler->clicked(this);
}

ListBoxItem::ListBoxItem(const std::string &text)
{
    m_qtitem = new QListWidgetItem(QString::fromStdString(text));
    m_qtitem->setData(Qt::UserRole, QVariant::fromValue<void *>(this));
}

ListBoxItem::~ListBoxItem()
{
    delete m_qtitem;
}

void ListBoxItem::set_text(const std::string &text)
{
    m_qtitem->setText(QString::fromStdString(text));
}

ListBox::ListBox() :
    m_handler(NULL)
{
    m_qtwidget = new QListWidget;
    connect(m_qtwidget, SIGNAL(itemActivated(QListWidgetItem *)),
            SLOT(itemActivated_slot(QListWidgetItem *)));
}

ListBox::~ListBox()
{
    delete m_qtwidget;
}

void ListBox::add_item(ListBoxItem *item)
{
    m_qtwidget->addItem(item->m_qtitem);
}

void ListBox::scroll_to(ListBoxItem *item)
{
    m_qtwidget->scrollToItem(item->m_qtitem);
}

QWidget *ListBox::qt_widget()
{
    return m_qtwidget;
}

void ListBox::itemActivated_slot(QListWidgetItem *qtitem)
{
    ListBoxItem *item = reinterpret_cast<ListBoxItem *>(
                        qtitem->data(Qt::UserRole).value<void *>());
    if (m_handler)
        m_handler->clicked(this, item);
}

Window::Window(const std::string &title) :
    m_title(title)
{
}

Window::~Window()
{
}

void Window::set_widget(Widget *widget)
{
    m_qtwidget = widget->qt_widget();
    m_qtwidget->setWindowTitle(QString::fromStdString(m_title));
}

void Window::show()
{
    m_qtwidget->show();
}

IoWatch::IoWatch(int fd) :
    m_handler(NULL)
{
    m_notifier = new QSocketNotifier(fd, QSocketNotifier::Read);
    connect(m_notifier, SIGNAL(activated(int)), SLOT(activated_slot()));
}

IoWatch::~IoWatch()
{
    delete m_notifier;
}

void IoWatch::activated_slot()
{
    if (m_handler)
        m_handler->ready(this);
}

Timer::Timer(int interval) :
    m_handler(NULL)
{
    m_id = startTimer(interval);
}

Timer::~Timer()
{
}

void Timer::timerEvent(QTimerEvent *event)
{
    if (event->timerId() != m_id)
        return;
    if (m_handler)
        m_handler->timeout(this);
}

Application::Application(int *argc, char ***argv)
{
    if (m_instance)
        throw std::runtime_error("Application instance already created");
    m_instance = this;
    new QApplication(*argc, *argv);
}

void Application::quit()
{
    qApp->quit();
}

int Application::run()
{
    return qApp->exec();
}
