#-------------------------------------------------
#
# Project created by QtCreator 2012-07-27T11:37:10
#
#-------------------------------------------------

TARGET = RtcComponent
TEMPLATE = lib

DEFINES += RTCCOMPONENT_LIBRARY

SOURCES += rtcfactory.cpp \
    rtclogic.cpp

HEADERS += rtcfactory.h\
        RtcComponent_global.h \
    rtclogic.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE2C4730D
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = RtcComponent.dll
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

QMAKE_CFLAGS_DEBUG += -std=gnu99
QMAKE_CFLAGS_RELEASE += -std=gnu99

unix:!macx:!symbian: LIBS += -L$$OUT_PWD/../TwiComponent/ -lTwiComponent

INCLUDEPATH += $$PWD/../TwiComponent
DEPENDPATH += $$PWD/../TwiComponent

unix:!macx:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../TwiComponent/libTwiComponent.a
