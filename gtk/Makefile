LDFLAGS = `pkg-config gtk+-2.0 --libs`
CXXFLAGS = -g `pkg-config gtk+-2.0 --cflags` -DTINYUI_GTK -W -Wall -O2 -I..
OBJS = ../test.cc tinyui.o ../common.o ../unix.o

all: test

test: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS)
