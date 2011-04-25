#include "tiny_ui.h"

namespace tinyui {

void Button::set_handler(ClickInterface *events)
{
	m_handler = events;
}

void ListBox::set_handler(ListBoxInterface *handler)
{
	m_handler = handler;
}

void IoWatch::set_handler(IoWatchInterface *events)
{
	m_handler = events;
}

void Timer::set_handler(TimerInterface *events)
{
	m_handler = events;
}

void Entry::set_handler(EntryInterface *events)
{
	m_handler = events;
}

Application *Application::instance()
{
	return m_instance;
}

Application *Application::m_instance = NULL;

}
