#-------------------------------------------------
#
# Project created by QtCreator 2012-07-21T13:20:15
#
#-------------------------------------------------

TARGET = LcdComponent
TEMPLATE = lib

DEFINES += LCDCOMPONENT_LIBRARY

SOURCES += \
    lcdlogic.cpp \
    hd44780.c \
    lcdfactory.cpp \
    lcdgui.cpp

HEADERS += \
    lcdlogic.h \
    hd44780.h \
    lcdfactory.h \
    lcdgui.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE4B78C8E
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = LcdComponent.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

QMAKE_CFLAGS_DEBUG += -std=gnu99
QMAKE_CFLAGS_RELEASE += -std=gnu99

INCLUDEPATH += ../QSimAVR \
               /usr/include/simavr \
               /usr/include/simavr/avr
LIBS += -lsimavr

FORMS += \
    lcdgui.ui
