#-------------------------------------------------
#
# Project created by QtCreator 2016-06-05T18:01:55
#
# All Rights by Philipp Schepper 2016
#
#-------------------------------------------------

QT       += core gui printsupport network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Einsatzplaner
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    person.cpp \
    managerpersonal.cpp \
    activity.cpp \
    reservierung.cpp \
    manager.cpp \
    managerreservierungen.cpp \
    fahrtagwindow.cpp \
    activitywindow.cpp \
    personalwindow.cpp \
    fahrtag.cpp \
    calendar.cpp \
    calendarday.cpp \
    aactivity.cpp \
    export.cpp \
    exportgesamt.cpp \
    fileio.cpp

HEADERS  += mainwindow.h \
    person.h \
    managerpersonal.h \
    activity.h \
    reservierung.h \
    manager.h \
    managerreservierungen.h \
    fahrtagwindow.h \
    activitywindow.h \
    personalwindow.h \
    fahrtag.h \
    calendar.h \
    calendarday.h \
    aactivity.h \
    export.h \
    exportgesamt.h \
    fileio.h

FORMS    += mainwindow.ui \
#    managerreservierungen.ui \
    fahrtagwindow.ui \
    activitywindow.ui \
    personalwindow.ui \
    calendar.ui \
    calendarday.ui \
    exportgesamt.ui

CONFIG -= console

win32 {
    RC_ICONS = ../Icon/keks.ico
    QMAKE_TARGET_COMPANY = Philipp Schepper
    QMAKE_TARGET_DESCRIPTION = Ein Programm fuer Museumseisenbahnen
    QMAKE_TARGET_COPYRIGHT = Rechte bei Philipp Schepper 2016
    QMAKE_TARGET_PRODUCT = Einsatzplaner
    VERSION = 1.0.0.1
}

macx {
    VERSION = 1.0.0
    OTHER_FILES += Info.plist ../Icon/keks.icns
    QMAKE_INFO_PLIST = Info.plist
    DISTFILES += Info.plist
    # Bundle identifier for your application
    BUNDLEID = de.philipp-schepper.einsatzplaner
    ICON = ../Icon/keks.icns
    codesign.commands += macdeployqt $${TARGET}.app;
}
