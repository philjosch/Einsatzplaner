#-------------------------------------------------
#
# Developers:
# 2016-2020: Philipp Schepper
#
#-------------------------------------------------

!versionAtLeast(QT_VERSION, 5.13.0):error("Qt version 5.13 is required for this project")

win32: LCONVERT=$$[QT_INSTALL_BINS]\lconvert.exe
else:  LCONVERT=$$[QT_INSTALL_BINS]/lconvert
system(\"$$LCONVERT\" -o \"$$_PRO_FILE_PWD_/qt_de.qm\" \"$$[QT_INSTALL_TRANSLATIONS]/qtbase_de.qm\")

QT       += core gui printsupport network widgets

TARGET = Einsatzplaner
TEMPLATE = app


SOURCES += main.cpp\
    einstellungen.cpp \
    guihelper.cpp \
    mainwindow.cpp \
    networking.cpp \
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
    einstellungen.h \
    guihelper.h \
    networking.h \
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

CONFIG -= console

#OTHER_FILES += qt_de.qm

VERSION = 1.6.1
win32 {
    RC_ICONS = ../Icon/keks.ico
    QMAKE_TARGET_COMPANY = Philipp Schepper
    QMAKE_TARGET_DESCRIPTION = Ein Programm fuer Museumseisenbahnen
    QMAKE_TARGET_COPYRIGHT = Entwicklung von Philipp Schepper 2016-2020
    QMAKE_TARGET_PRODUCT = Einsatzplaner
    VERSION = 1.6.1.1
}

macx {
    OTHER_FILES += Info.plist ../Icon/keks.icns
    QMAKE_INFO_PLIST = Info.plist
    DISTFILES += Info.plist
    BUNDLEID = de.philipp-schepper.einsatzplaner
    ICON = ../Icon/keks.icns
}

RESOURCES += \
    resources.qrc

win32: COMMIT_HASH = $$system(git.exe -C \""$$_PRO_FILE_PWD_"\" rev-parse --short HEAD)
else:  COMMIT_HASH = $$system(git -C \""$$_PRO_FILE_PWD_"\" rev-parse --short HEAD)
DEFINES += GIT_CURRENT_SHA1="\"\\\"$${COMMIT_HASH}\\\"\""
