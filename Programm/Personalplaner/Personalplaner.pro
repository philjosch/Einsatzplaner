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

TRANSLATIONS += \
    Personalplaner_de_DE.ts

macx {
    BUNDLEID = de.philipp-schepper.einsatzplaner
    ICON = ../Icon/keks.icns
}
