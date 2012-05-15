#-------------------------------------------------
#
# Project created by QtCreator 2012-05-10T09:17:39
#
#-------------------------------------------------

QT       += core gui

TARGET = z80_gui
TEMPLATE = app


SOURCES += main.cpp\
    z80/z80.c \
    gui/mainwindow.cpp \
    gui/mylabel.cpp

HEADERS  += z80/z80.h \
    gui/mainwindow.h \
    z80/memory.h \
    z80/general.h \
    gui/mylabel.h

