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
    mainwindow.cpp \
    personalwindow.cpp \
    calendarday.cpp \
    exportdialog.cpp

HEADERS  += mainwindow.h \
    personalwindow.h \
    calendarday.h \
    exportdialog.h

FORMS    += mainwindow.ui \
    personalwindow.ui \
    calendarday.ui \
    exportdialog.ui
