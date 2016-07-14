QT       += testlib gui

INCLUDEPATH += ../src

CONFIG   += console c++11
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
           mainwindowtest.cpp \
           duplicatesfindertest.cpp \
           ../src/duplicatesfinder.cpp

HEADERS  += mainwindowtest.h \
            duplicatesfindertest.h \
            ../src/duplicatesfinder.h
