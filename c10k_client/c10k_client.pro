TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lpthread
INCLUDEPATH += ./include

SOURCES += \
    src/sysdata.cpp \
    src/epoll_cli.cpp \
    src/main.cpp \
    src/wrap.cpp

HEADERS += \
    include/sysdata.h \
    include/epoll_cli.h \
    include/wrap.h
