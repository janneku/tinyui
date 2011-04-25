unix: isEmpty(PREFIX) error(Prefix is not set. Please run ./configure instead!)
TEMPLATE = lib
TARGET = tinyui-qt
DEPENDPATH += .
INCLUDEPATH += ..
DEFINES += TINYUI_QT

unix: {
    exists($$PREFIX/lib64) INSTALL_PATH = $$PREFIX/lib64
    isEmpty(INSTALL_PATH) INSTALL_PATH = $$PREFIX/lib

    target.path = $$INSTALL_PATH
    headers.path = $$PREFIX/include/tinyui
}

headers.files += ../tiny_ui.h
INSTALLS += target headers

HEADERS += ../tiny_ui.h
SOURCES += tinyui.cc ../common.cc ../unix.cc
