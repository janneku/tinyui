#include "tiny_ui.h"
#include <stdexcept>
#include <sys/signal.h>
#include <sys/socket.h>
#include <stdio.h>
#include <cstdlib>

SigIntHandler *SigIntHandler::m_instance = NULL;

SigIntHandler::SigIntHandler() :
    m_handler(NULL)
{
    if (m_instance)
        throw std::runtime_error("SIGINT handler already installed");
    m_instance = this;

    socketpair(AF_UNIX, SOCK_STREAM, 0, m_fd);

    struct sigaction term;
    term.sa_handler = signal_handler;
    sigemptyset(&term.sa_mask);
    term.sa_flags = SA_RESTART;
    sigaction(SIGINT, &term, 0);
    sigaction(SIGTERM, &term, 0);
    sigaction(SIGHUP, &term, 0);

    IoWatch *watch = new IoWatch(m_fd[1]);
    watch->set_handler(this);
}

void SigIntHandler::set_handler(QuitInterface *handler)
{
    m_handler = handler;
}

void SigIntHandler::ready(IoWatch *iowatch)
{
    UNUSED(iowatch);
    char a;
    ssize_t ret = read(m_fd[1], &a, 1);
    UNUSED(ret);
    if (m_handler)
        m_handler->quit();
}

void SigIntHandler::signal_handler(int sig)
{
    UNUSED(sig);
    char a = 0;
    if (write(m_instance->m_fd[0], &a, 1) < 1) {
        fprintf(stderr, "Unable to write to signal pipe\n");
        abort();
    }
}
