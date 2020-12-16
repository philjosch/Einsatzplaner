#-------------------------------------------------
#
# Developers:
# 2016-2020: Philipp Schepper
#
#-------------------------------------------------

include(../share.pri)


TARGET = Einsatzplaner
TEMPLATE = app


SOURCES += main.cpp\
    guihelper.cpp \
    mainwindow.cpp \
    fahrtagwindow.cpp \
    activitywindow.cpp \
    personalwindow.cpp \
    calendarday.cpp \
    exportdialog.cpp \
    coreapplication.cpp \
    einstellungendialog.cpp \
    minimumhourseditordialog.cpp \
    filesettingsdialog.cpp

HEADERS  += mainwindow.h \
    guihelper.h \
    fahrtagwindow.h \
    activitywindow.h \
    personalwindow.h \
    calendarday.h \
    exportdialog.h \
    coreapplication.h \
    einstellungendialog.h \
    minimumhourseditordialog.h \
    filesettingsdialog.h

FORMS    += mainwindow.ui \
    fahrtagwindow.ui \
    activitywindow.ui \
    personalwindow.ui \
    calendarday.ui \
    exportdialog.ui \
    einstellungendialog.ui \
    minimumhourseditordialog.ui \
    filesettingsdialog.ui

win32 {
    RC_ICONS = ../Icon/keks.ico
    QMAKE_TARGET_COMPANY = Philipp Schepper
    QMAKE_TARGET_DESCRIPTION = Ein Programm fuer Museumseisenbahnen
    QMAKE_TARGET_COPYRIGHT = Entwicklung von Philipp Schepper 2016-2020
    QMAKE_TARGET_PRODUCT = Einsatzplaner
    VERSION = 1.6.3.1
}

macx {
    OTHER_FILES += Info.plist ../Icon/keks.icns
    QMAKE_INFO_PLIST = Info.plist
    DISTFILES += Info.plist
    BUNDLEID = de.philipp-schepper.einsatzplaner
    ICON = ../Icon/keks.icns
}



