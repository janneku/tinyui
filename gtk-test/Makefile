CXXFLAGS = -g `pkg-config gtk+-2.0 --cflags` -DTINYUI_GTK -W -Wall -O2
EXEFLAGS = -g `pkg-config gtk+-2.0 --libs` -ltinyui-gtk
OBJS = ../test.o

all: test

test: $(OBJS)
	$(CXX) $(EXEFLAGS) -o $@ $(OBJS)
