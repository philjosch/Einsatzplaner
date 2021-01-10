#-------------------------------------------------
#
# Developers:
# 2016-2021: Philipp Schepper
#
#-------------------------------------------------

include(../share.pri)


TARGET = Einsatzplaner
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    fahrtagwindow.cpp \
    activitywindow.cpp \
    personalwindow.cpp \
    calendarday.cpp \
    exportdialog.cpp \
    filesettingsdialog.cpp

HEADERS  += mainwindow.h \
    fahrtagwindow.h \
    activitywindow.h \
    personalwindow.h \
    calendarday.h \
    exportdialog.h \
    filesettingsdialog.h

FORMS    += mainwindow.ui \
    fahrtagwindow.ui \
    activitywindow.ui \
    personalwindow.ui \
    calendarday.ui \
    exportdialog.ui \
    filesettingsdialog.ui

win32 {
    QMAKE_TARGET_PRODUCT = Einsatzplaner
}

macx {
    BUNDLEID = de.philipp-schepper.einsatzplaner
    ICON = $$PWD/../../Icon/Einsatzplaner.icns
}



