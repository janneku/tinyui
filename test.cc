#include "tiny_ui.h"
#include <stdio.h>

class MyWindow: public tinyui::Window, tinyui::ClickInterface,
		tinyui::IoWatchInterface, tinyui::TimerInterface,
		tinyui::ListBoxInterface, tinyui::EntryInterface
{
public:
	MyWindow() :
		Window(L"Hello world \x3A9"),
		m_mainbox(tinyui::VERTICAL),
		m_quitbutton(L"Quit"),
		m_watch(0),
		m_timer(1000),
		m_entry(L"Please write here")
	{
		for (int i = 0; i < 5; ++i) {
			m_items[i].set_text(L"item \x3A9 " + tinyui::format_number(i));
			m_listbox.add_item(&m_items[i]);

			m_buttons[i].set_label(L"label " + tinyui::format_number(i));
			m_buttons[i].set_handler(this);
			m_mainbox.add_widget(&m_buttons[i]);
		}
		m_entry.set_handler(this);
		m_listbox.set_handler(this);
		m_quitbutton.set_handler(this);
		m_watch.set_handler(this);
		m_timer.set_handler(this);
		m_mainbox.add_widget(&m_entry);
		m_mainbox.add_widget(&m_listbox);
		m_mainbox.add_widget(&m_quitbutton);
		set_widget(&m_mainbox);
	}

private:
	tinyui::BoxLayout m_mainbox;
	tinyui::ListBox m_listbox;
	tinyui::ListBoxItem m_items[5];
	tinyui::Button m_buttons[5];
	tinyui::Button m_quitbutton;
	tinyui::IoWatch m_watch;
	tinyui::Timer m_timer;
	tinyui::Entry m_entry;

	void clicked(tinyui::Button *button)
	{
		if (button == &m_quitbutton)
			tinyui::Application::instance()->quit();
	}
	void ready(tinyui::IoWatch *watch)
	{
		UNUSED(watch);
		char buf[256];
		ssize_t len = read(0, buf, sizeof(buf) - 1);
		if (len == 0)
			tinyui::Application::instance()->quit();
		while (len > 0 && buf[len - 1] == '\n')
			len--;
		buf[len] = 0;
		printf("<%s>\n", buf);
	}
	void timeout(tinyui::Timer *timer)
	{
		UNUSED(timer);
		printf("tick!\n");
	}
	void clicked(tinyui::ListBox *listbox, tinyui::ListBoxItem *item)
	{
		UNUSED(listbox);
		for (int i = 0; i < 5; ++i) {
			if (&m_items[i] == item)
				printf("%d clicked\n", i);
		}
	}
	void activated(tinyui::Entry *entry)
	{
		std::wstring s = entry->get_text();
		printf("entry: <%s>\n", tinyui::encode_utf8(s).c_str());
	}
};

int main(int argc, char **argv)
{
	tinyui::Application app(&argc, &argv);
	tinyui::SigIntHandler sighandler;
	sighandler.set_handler(&app);
	MyWindow mw;
	mw.show();
	return app.run();
}
