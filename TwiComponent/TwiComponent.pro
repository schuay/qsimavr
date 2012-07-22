#-------------------------------------------------
#
# Project created by QtCreator 2012-07-27T12:11:18
#
#-------------------------------------------------

TARGET = TwiComponent
TEMPLATE = lib
CONFIG += staticlib

DEFINES += TWICOMPONENT_LIBRARY

SOURCES += twicomponent.cpp

HEADERS += twicomponent.h\
        TwiComponent_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE2B2A674
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = TwiComponent.dll
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

INCLUDEPATH += ../QSimAVR \
               /usr/include/simavr \
               /usr/include/simavr/avr
LIBS += -lsimavr
