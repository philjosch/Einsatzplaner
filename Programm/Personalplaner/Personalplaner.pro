#-------------------------------------------------
#
# Developers:
# 2016-2021: Philipp Schepper
#
#-------------------------------------------------

TARGET = Personalplaner
TARGET_KLEIN = personalplaner


include(../share.pri)


SOURCES += \
    main.cpp \
    mainwindowpersonal.cpp \
    personwindow.cpp

HEADERS += \
    mainwindowpersonal.h \
    personwindow.h

FORMS += \
    mainwindowpersonal.ui \
    personwindow.ui


#TRANSLATIONS += \
#    Personalplaner_de_DE.ts
