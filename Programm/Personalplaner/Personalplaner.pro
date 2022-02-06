#-------------------------------------------------
#
# Developers:
# 2016-2022: Philipp Schepper
#
#-------------------------------------------------

TARGET = Personalplaner
TARGET_KLEIN = personalplaner


include(../share.pri)


SOURCES += \
    beitraegeeditordialog.cpp \
    main.cpp \
    mainwindowpersonal.cpp

HEADERS += \
    beitraegeeditordialog.h \
    mainwindowpersonal.h

FORMS += \
    beitraegeeditordialog.ui \
    mainwindowpersonal.ui


#TRANSLATIONS += \
#    Personalplaner_de_DE.ts
