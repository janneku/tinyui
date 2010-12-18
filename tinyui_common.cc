#include "tiny_ui.h"

void Button::set_events(ButtonEvents *events)
{
    m_events = events;
}

void IoWatch::set_events(IoWatchEvents *events)
{
    m_events = events;
}

void Timer::set_events(TimerEvents *events)
{
    m_events = events;
}

Application *Application::instance()
{
    return m_instance;
}

Application *Application::m_instance;

