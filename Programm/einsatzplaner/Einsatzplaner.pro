#-------------------------------------------------
#
# Developers:
# 2016-2022: Philipp Schepper
#
#-------------------------------------------------

TARGET = Einsatzplaner
TARGET_KLEIN = einsatzplaner


include(../share.pri)


SOURCES += main.cpp\
    mainwindowevents.cpp \
    personalwindow.cpp \
    calendarday.cpp \
    exportdialog.cpp

HEADERS  += \
    mainwindowevents.h \
    personalwindow.h \
    calendarday.h \
    exportdialog.h

FORMS    += \
    mainwindowevents.ui \
    personalwindow.ui \
    calendarday.ui \
    exportdialog.ui
