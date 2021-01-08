#-------------------------------------------------
#
# Developers:
# 2016-2021: Philipp Schepper
#
#-------------------------------------------------

include(../share.pri)

TARGET = Personalplaner
TEMPLATE = app


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

win32 {
    QMAKE_TARGET_PRODUCT = Personalplaner
}
macx {
    BUNDLEID = de.philipp-schepper.personalplaner
}
