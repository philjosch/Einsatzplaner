#-------------------------------------------------
#
# Developers:
# 2016-2021: Philipp Schepper
#
#-------------------------------------------------

TARGET = Einsatzplaner
TARGET_KLEIN = einsatzplaner


include(../share.pri)


SOURCES += main.cpp\
    mainwindow.cpp \
    fahrtagwindow.cpp \
    activitywindow.cpp \
    personalwindow.cpp \
    calendarday.cpp \
    exportdialog.cpp

HEADERS  += mainwindow.h \
    fahrtagwindow.h \
    activitywindow.h \
    personalwindow.h \
    calendarday.h \
    exportdialog.h

FORMS    += mainwindow.ui \
    fahrtagwindow.ui \
    activitywindow.ui \
    personalwindow.ui \
    calendarday.ui \
    exportdialog.ui
