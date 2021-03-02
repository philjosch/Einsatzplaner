#-------------------------------------------------
#
# Developers:
# 2016-2021: Philipp Schepper
#
#-------------------------------------------------
TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++11
CONFIG += console

QT       += core printsupport network


SOURCES += einstellungen.cpp \
    eplfile.cpp \
    networking.cpp \
    person.cpp \
    managerpersonal.cpp \
    reservierung.cpp \
    manager.cpp \
    fahrtag.cpp \
    aactivity.cpp \
    export.cpp \
    fileio.cpp \
    wagen.cpp \
    verteiler.cpp \
    basics.cpp \
    version.cpp \
    filesettings.cpp

HEADERS  += einstellungen.h \
    eplexception.h \
    eplfile.h \
    networking.h \
    person.h \
    managerpersonal.h \
    reservierung.h \
    manager.h \
    fahrtag.h \
    aactivity.h \
    export.h \
    fileio.h \
    wagen.h \
    verteiler.h \
    basics.h \
    version.h \
    filesettings.h

INCLUDEPATH += ../crypto
LIBS += -L../crypto -lcrypto

DEPENDPATH +=      $$PWD/../crypto
PRE_TARGETDEPS +=  $$PWD/../crypto


# Default rules for deployment.
#unix {
#    target.path = $$[QT_INSTALL_PLUGINS]/generic
#}
#!isEmpty(target.path): INSTALLS += target
