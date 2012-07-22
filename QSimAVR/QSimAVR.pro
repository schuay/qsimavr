#-------------------------------------------------
#
# Project created by QtCreator 2012-07-20T17:18:09
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qsimavr
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    simavr.cpp \
    QsLogDest.cpp \
    QsLog.cpp \
    QsDebugOutput.cpp \
    pluginmanager.cpp \
    recentfiles.cpp

HEADERS  += mainwindow.h \
    simavr.h \
    component.h \
    QsLogLevel.h \
    QsLogDest.h \
    QsLog.h \
    QsDebugOutput.h \
    pluginmanager.h \
    recentfiles.h

FORMS    += mainwindow.ui

INCLUDEPATH += /usr/include/simavr \
               /usr/include/simavr/avr
LIBS += -lsimavr -lelf
