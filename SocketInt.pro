#-------------------------------------------------
#
# Project created by QtCreator 2015-09-18T14:16:14
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SocketInt
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    sockethost.cpp \
    socketclient.cpp

HEADERS  += mainwindow.h \
    sockethost.h \
    socketclient.h

FORMS    += mainwindow.ui
