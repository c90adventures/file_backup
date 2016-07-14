QT       += testlib qml
QT       -= gui

INCLUDEPATH += ../src

CONFIG   += console c++11
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp\
           mainwindowtest.cpp

HEADERS  += mainwindowtest.h
