#-------------------------------------------------
#
# Project created by QtCreator 2012-07-25T18:40:59
#
#-------------------------------------------------

TARGET = EepromComponent
TEMPLATE = lib

DEFINES += EEPROMCOMPONENT_LIBRARY

SOURCES += eepromfactory.cpp \
    eepromlogic.cpp

HEADERS += eepromfactory.h \
    eepromlogic.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE1E279FD
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = EepromComponent.dll
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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../TwiComponent/release/ -lTwiComponent
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../TwiComponent/debug/ -lTwiComponent
else:unix:!symbian: LIBS += -L$$OUT_PWD/../TwiComponent/ -lTwiComponent

INCLUDEPATH += $$PWD/../TwiComponent
DEPENDPATH += $$PWD/../TwiComponent

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../TwiComponent/release/TwiComponent.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../TwiComponent/debug/TwiComponent.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../TwiComponent/libTwiComponent.a
