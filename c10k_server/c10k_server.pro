TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lpthread
INCLUDEPATH += ./include

SOURCES += \
    src/data.cpp \
    src/epoll_ser.cpp \
    src/main.cpp \
    src/threadpool.cpp \
    src/wrap.cpp

HEADERS += \
    include/data.h \
    include/epoll_ser.h \
    include/threadpool.h \
    include/wrap.h
