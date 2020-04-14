#-------------------------------------------------
#
# Developers:
# 2016-2020: Philipp Schepper
#
#-------------------------------------------------

!versionAtLeast(QT_VERSION, 5.13.0):error("Qt version 5.13 is required for this project")

QT       += core gui printsupport network widgets

TARGET = Einsatzplaner
TEMPLATE = app

SOURCES += main.cpp\
    mainwindow.cpp \
    person.cpp \
    managerpersonal.cpp \
    activity.cpp \
    reservierung.cpp \
    manager.cpp \
    fahrtagwindow.cpp \
    activitywindow.cpp \
    personalwindow.cpp \
    fahrtag.cpp \
    calendarday.cpp \
    aactivity.cpp \
    export.cpp \
    exportgesamt.cpp \
    fileio.cpp \
    coreapplication.cpp \
    wagen.cpp \
    verteiler.cpp \
    preferencesdialog.cpp \
    minimumhourseditordialog.cpp \
    basics.cpp \
    filesettings.cpp \
    managerfilesettings.cpp

HEADERS  += mainwindow.h \
    person.h \
    managerpersonal.h \
    activity.h \
    reservierung.h \
    manager.h \
    fahrtagwindow.h \
    activitywindow.h \
    personalwindow.h \
    fahrtag.h \
    calendarday.h \
    aactivity.h \
    export.h \
    exportgesamt.h \
    fileio.h \
    coreapplication.h \
    wagen.h \
    verteiler.h \
    preferencesdialog.h \
    minimumhourseditordialog.h \
    basics.h \
    filesettings.h \
    managerfilesettings.h

FORMS    += mainwindow.ui \
    fahrtagwindow.ui \
    activitywindow.ui \
    personalwindow.ui \
    calendarday.ui \
    exportgesamt.ui \
    preferencesdialog.ui \
    minimumhourseditordialog.ui \
    filesettings.ui

TRANSLATIONS += einsatzplaner_de.ts

CONFIG -= console

OTHER_FILES += qt_de.qm einsatzplaner_de.qm

win32 {
    RC_ICONS = ../Icon/keks.ico
    QMAKE_TARGET_COMPANY = Philipp Schepper
    QMAKE_TARGET_DESCRIPTION = Ein Programm fuer Museumseisenbahnen
    QMAKE_TARGET_COPYRIGHT = Entwicklung von Philipp Schepper 2016-2020
    QMAKE_TARGET_PRODUCT = Einsatzplaner
    VERSION = 1.6.0.1
}

macx {
    VERSION = 1.6.0
    OTHER_FILES += Info.plist ../Icon/keks.icns
    QMAKE_INFO_PLIST = Info.plist
    DISTFILES += Info.plist
    BUNDLEID = de.philipp-schepper.einsatzplaner
    ICON = ../Icon/keks.icns
}

RESOURCES += \
    resources.qrc
