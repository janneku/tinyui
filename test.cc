#include "tiny_ui.h"
#include <stdio.h>

class MyWindow: public Window, public ButtonEvents, public IoWatchEvents
{
public:
    MyWindow() :
        Window("Hello world"),
        m_mainbox(VERTICAL),
        m_quitbutton("Quit"),
        m_watch(0)
    {
        for (int i = 0; i < 5; ++i) {
            m_buttons[i].set_label("label " + format_number(i));
            m_buttons[i].set_events(this);
            m_mainbox.add_widget(&m_buttons[i]);
        }
        m_quitbutton.set_events(this);
        m_watch.set_events(this);
        m_mainbox.add_widget(&m_quitbutton);
        set_widget(&m_mainbox);
    }

private:
    BoxLayout m_mainbox;
    Button m_buttons[5];
    Button m_quitbutton;
    IoWatch m_watch;

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
};

int main(int argc, char **argv)
{
    Application app(&argc, &argv);
    MyWindow mw;
    mw.show();
    return app.run();
}
