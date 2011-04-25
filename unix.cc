#include "tiny_ui.h"
#include <stdexcept>
#include <sys/signal.h>
#include <sys/socket.h>
#include <stdio.h>
#include <cstdlib>

namespace tinyui {

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

	IoWatch *watch = new IoWatch(m_fd[1], IN);
	watch->set_handler(this);
}

void SigIntHandler::set_handler(QuitInterface *handler)
{
	m_handler = handler;
}

void SigIntHandler::ready(IoWatch *iowatch, IoDirection dir)
{
	UNUSED(iowatch);
	UNUSED(dir);
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

std::string encode_utf8(const std::wstring &in)
{
	std::string out;
	for (size_t i = 0; i < in.size(); ++i) {
		unsigned int c = in[i];
		if (c <= 0x7f) {
			out += static_cast<char>(c);
		} else if (c <= 0x7ff) {
			out += static_cast<char>(0xc0 | ((c >> 6) & 0x1f));
			out += static_cast<char>(0x80 | (c & 0x3f));
		} else if (c <= 0xffff) {
			out += static_cast<char>(0xe0 | ((c >> 12) & 0x0f));
			out += static_cast<char>(0x80 | ((c >> 6) & 0x3f));
			out += static_cast<char>(0x80 | (c & 0x3f));
		} else {
			out += static_cast<char>(0xf0 | ((c >> 18) & 0x07));
			out += static_cast<char>(0x80 | ((c >> 12) & 0x3f));
			out += static_cast<char>(0x80 | ((c >> 6) & 0x3f));
			out += static_cast<char>(0x80 | (c & 0x3f));
		}
	}
	return out;
}

std::wstring decode_utf8(const std::string &in)
{
	std::wstring out;
	for (size_t i = 0; i < in.size(); ++i) {
		unsigned char c = in[i];
		unsigned int wc = 0;
		if ((c & 0xf0) == 0xf0) {
			if (i + 3 >= in.size())
				break;
			wc = (static_cast<unsigned char>(c & ~0xF0) << 18) +
			     (static_cast<unsigned char>(in[i + 1] & ~0x80) << 12) +
			     (static_cast<unsigned char>(in[i + 2] & ~0x80) << 6) +
			     static_cast<unsigned char>(in[i + 3] & ~0x80);
			i += 2;
		} else if ((c & 0xe0) == 0xe0) {
			if (i + 2 >= in.size())
				break;
			wc = (static_cast<unsigned char>(c & ~0xE0) << 12) +
			     (static_cast<unsigned char>(in[i + 1] & ~0x80) << 6) +
			      static_cast<unsigned char>(in[i + 2] & ~0x80);
			i += 1;
		} else if ((c & 0xc0) == 0xc0) {
			if (i + 1 >= in.size())
				break;
			wc = (static_cast<unsigned char>(c & ~0xC0) << 6) +
			      static_cast<unsigned char>(in[i + 1] & ~0x80);
			i++;
		} else if (c <= 0x7f)
			wc = c;
		if (static_cast<wchar_t>(wc) == wc)
			out += static_cast<wchar_t>(wc);
	}
	return out;
}

}
