#include "tiny_ui.h"
#include <stdexcept>
#include <QBoxLayout>
#include <QPushButton>
#include <QSocketNotifier>
#include <QApplication>

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
    delete m_qtlayout;
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
    m_events(NULL)
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
void Button::set_events(ButtonEvents *events)
{
    m_events = events;
}

QWidget *Button::qt_widget()
{
    return m_qtwidget;
}

void Button::clicked_slot()
{
    if (m_events)
        m_events->clicked(this);
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
    m_events(NULL)
{
    m_notifier = new QSocketNotifier(fd, QSocketNotifier::Read);
    connect(m_notifier, SIGNAL(activated(int)), SLOT(activated_slot()));
}

IoWatch::~IoWatch()
{
    delete m_notifier;
}

void IoWatch::set_events(IoWatchEvents *events)
{
    m_events = events;
}

void IoWatch::activated_slot()
{
    if (m_events)
        m_events->ready(this);
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

Application *Application::instance()
{
    return m_instance;
}

Application *Application::m_instance;

