#-------------------------------------------------
#
# Project created by QtCreator 2015-03-06T16:57:47
#
#-------------------------------------------------

QT       += core gui

QT += widgets

TARGET = Einsatzplaner
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    export.cpp \
    qfahrtag.cpp

HEADERS  += mainwindow.h \
    export.h \
    qfahrtag.h

FORMS    += mainwindow.ui \
    export.ui

CONFIG += console
