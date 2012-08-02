#-------------------------------------------------
#
# Project created by QtCreator 2012-08-01T17:57:03
#
#-------------------------------------------------

TARGET = GlcdComponent
TEMPLATE = lib

DEFINES += GLCDCOMPONENT_LIBRARY

SOURCES += glcdfactory.cpp \
    glcdlogic.cpp \
    nt7108.cpp

HEADERS += glcdfactory.h\
        GlcdComponent_global.h \
    glcdlogic.h \
    nt7108.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE0D7BE62
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = GlcdComponent.dll
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
