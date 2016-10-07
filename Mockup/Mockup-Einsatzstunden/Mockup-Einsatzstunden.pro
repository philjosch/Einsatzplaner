#-------------------------------------------------
#
# Project created by QtCreator 2016-04-20T17:57:07
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Mockup-Einsatzstunden
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    arbeitseinatz.cpp \
    personal.cpp

HEADERS  += mainwindow.h \
    arbeitseinatz.h \
    personal.h

FORMS    += mainwindow.ui \
    arbeitseinatz.ui \
    personal.ui
macx {
    codesign.commands += macdeployqt $${TARGET}.app;
}
