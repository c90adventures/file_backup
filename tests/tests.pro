QT       += testlib widgets gui

INCLUDEPATH += ../src

CONFIG   += console c++11
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
           mainwindowtest.cpp \
           ../src/mainwindow.cpp

HEADERS  += mainwindowtest.h \
            ../src/mainwindow.h

FORMS   += ../src/mainwindow.ui
