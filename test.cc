#include "tiny_ui.h"
#include <stdio.h>

class MyWindow: public Window, private ClickInterface, private IoWatchInterface,
                private TimerInterface, private ListBoxInterface
{
public:
    MyWindow() :
        Window(L"Hello world \x3A9"),
        m_mainbox(VERTICAL),
        m_quitbutton(L"Quit"),
        m_watch(0),
        m_timer(1000)
    {
        for (int i = 0; i < 5; ++i) {
            m_items[i].set_text(L"item \x3A9 " + format_number(i));
            m_listbox.add_item(&m_items[i]);

            m_buttons[i].set_label(L"label " + format_number(i));
            m_buttons[i].set_handler(this);
            m_mainbox.add_widget(&m_buttons[i]);
        }
        m_listbox.set_handler(this);
        m_quitbutton.set_handler(this);
        m_watch.set_handler(this);
        m_timer.set_handler(this);
        m_mainbox.add_widget(&m_listbox);
        m_mainbox.add_widget(&m_quitbutton);
        set_widget(&m_mainbox);
    }

private:
    BoxLayout m_mainbox;
    ListBox m_listbox;
    ListBoxItem m_items[5];
    Button m_buttons[5];
    Button m_quitbutton;
    IoWatch m_watch;
    Timer m_timer;

    void clicked(Button *button)
    {
        if (button == &m_quitbutton)
            Application::instance()->quit();
    }
    void ready(IoWatch *watch)
    {
        UNUSED(watch);
        char buf[256];
        ssize_t len = read(0, buf, sizeof(buf) - 1);
        if (len == 0)
            Application::instance()->quit();
        while (len > 0 && buf[len - 1] == '\n')
            len--;
        buf[len] = 0;
        printf("<%s>\n", buf);
    }
    void timeout(Timer *timer)
    {
        UNUSED(timer);
        printf("tick!\n");
    }
    void clicked(ListBox *listbox, ListBoxItem *item)
    {
        UNUSED(listbox);
        for (int i = 0; i < 5; ++i) {
            if (&m_items[i] == item)
                printf("%d clicked\n", i);
        }
    }
};

int main(int argc, char **argv)
{
    Application app(&argc, &argv);
    SigIntHandler sighandler;
    sighandler.set_handler(&app);
    MyWindow mw;
    mw.show();
    return app.run();
}
