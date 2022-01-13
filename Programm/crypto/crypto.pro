#-------------------------------------------------
#
# Developers:
# 2021-2022: Philipp Schepper
#
#-------------------------------------------------
TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++11
CONFIG += console
win32 {
    CONFIG -= debug_and_release debug_and_release_target
}

QT       += core

SOURCES += $$PWD/qt-aes/qaesencryption.cpp \
    crypto.cpp

HEADERS  += $$PWD/qt-aes/qaesencryption.h \
    crypto.h

# Default rules for deployment.
#unix {
#    target.path = $$[QT_INSTALL_PLUGINS]/generic
#}
#!isEmpty(target.path): INSTALLS += target
