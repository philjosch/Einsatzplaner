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
win32 {
    CONFIG -= debug_and_release debug_and_release_target
}

QT       += core printsupport network gui widgets


SOURCES += \
    guihelper.cpp \
    coremainwindow.cpp \
    einstellungendialog.cpp \
    minimumhourseditordialog.cpp \
    filesettingsdialog.cpp \
    coreapplication.cpp

HEADERS += \
    guihelper.h \
    coremainwindow.h \
    einstellungendialog.h \
    minimumhourseditordialog.h \
    filesettingsdialog.h \
    coreapplication.h

FORMS += \
    minimumhourseditordialog.ui \
    filesettingsdialog.ui \
    einstellungendialog.ui

INCLUDEPATH += ../crypto ../epl_api
LIBS += -L../crypto -lcrypto -L../epl_api -lepl_api

DEPENDPATH +=      $$PWD/../crypto $$PWD/../epl_api
PRE_TARGETDEPS +=  $$PWD/../crypto $$PWD/../epl_api

# Default rules for deployment.
#unix {
#    target.path = $$[QT_INSTALL_PLUGINS]/generic
#}
#!isEmpty(target.path): INSTALLS += target
