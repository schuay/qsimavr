#-------------------------------------------------
#
# Project created by QtCreator 2012-07-21T12:17:15
#
#-------------------------------------------------

TARGET = LedButtonsComponent
TEMPLATE = lib

DEFINES += LEDBUTTONSCOMPONENT_LIBRARY

SOURCES += \
    ledbuttonslogic.cpp \
    ledbuttonsgui.cpp \
    ledbuttonfactory.cpp

HEADERS +=\
    ledbuttonslogic.h \
    ledbuttonsgui.h \
    ledbuttonfactory.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE79012AB
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = LedButtonsComponent.dll
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

FORMS += \
    ledbuttonsgui.ui
