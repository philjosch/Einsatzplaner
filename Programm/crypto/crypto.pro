#-------------------------------------------------
#
# Developers:
# 2021: Philipp Schepper
#
#-------------------------------------------------
TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++11
CONFIG += console

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
