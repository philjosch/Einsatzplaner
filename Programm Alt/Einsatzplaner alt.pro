#-------------------------------------------------
#
# Project created by QtCreator 2015-03-06T16:57:47
#
# All Rights by Philipp Schepper 2016
#
#-------------------------------------------------

QT       += core gui widgets printsupport

TARGET = Einsatzplaner
TEMPLATE = app

SOURCES += main.cpp\
    export.cpp \
    planerfahrtage.cpp \
    planerfahrtagerahmendaten.cpp \
    planerfahrtagefahrplan.cpp \
    planerfahrtagepersonal.cpp \
    planerfahrtagereservierungen.cpp \
    planerfahrtageeinausgabe.cpp \
    planerfahrtagehilfsmethoden.cpp \
    planerfahrtagemenu.cpp \
    reservierung.cpp \
    managerreservierungen.cpp \
    fahrtag.cpp \
    managerzuege.cpp

HEADERS  += \
    export.h \
    planerfahrtage.h \
    reservierung.h \
    managerreservierungen.h \
    fahrtag.h \
    managerzuege.h

FORMS    += \
    export.ui \
    planerfahrtage.ui

CONFIG -= console


win32 {
    RC_ICONS = keks.ico
    QMAKE_TARGET_COMPANY = Philipp Schepper
    QMAKE_TARGET_DESCRIPTION = Ein Programm fuer Museumseisenbahnen
    QMAKE_TARGET_COPYRIGHT = Rechte bei Philipp Schepper 2016
    QMAKE_TARGET_PRODUCT = Einsatzplaner
    RC_ICONS = keks.ico
#    RC_LANG = German
    VERSION = 1.0.0.1
#    OTHER_FILES += keks.ico
}

macx {
    VERSION = 1.0.0

    OTHER_FILES += Info.plist keks.icns

    QMAKE_INFO_PLIST = Info.plist

    DISTFILES += Info.plist

    # Bundle identifier for your application
    BUNDLEID = de.philipp-schepper.einsatzplaner
    ICON = keks.icns
    codesign.commands += macdeployqt $${TARGET}.app;

}

#    # Name of the application signing certificate
#    APPCERT = "3rd Party Mac Developer Application: <yourcompany>"

#    # Name of the installer signing certificate
#    INSTALLERCERT = "3rd Party Mac Developer Installer: <yourcompany>"

#    # Bundle identifier for your application
#    BUNDLEID = com.yourcompany.MyApp

#    # Name of the entitlements file (only needed if you want to sandbox the application)
#    ENTITLEMENTS = Entitlements.plist

#    ICON = MyApp.icns

#     QMAKE_CFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
#    QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO
#    QMAKE_OBJECTIVE_CFLAGS_RELEASE =  $$QMAKE_OBJECTIVE_CFLAGS_RELEASE_WITH_DEBUGINFO
#    QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO

#    QMAKE_INFO_PLIST = MyAppInfo.plist
#    OTHER_FILES += MyAppInfo.plist
#                 $${ENTITLEMENTS}

#    codesign.depends  += all

#    # Remove unneeded frameworks (uncomment and change to suit your application)
#    #codesign.commands += rm -r $${TARGET}.app/Contents/Frameworks/QtDeclarative.framework;

#    # Remove unneeded plug-ins (uncomment and change to suit your application)
#    #codesign.commands += rm -r $${TARGET}.app/Contents/PlugIns/accessible;

#    # Extract debug symbols
#    codesign.commands += dsymutil $${TARGET}.app/Contents/MacOS/$${TARGET} -o $${TARGET}.app.dSYM;

#    # Sign frameworks and plug-ins (uncomment and change to suit your application)
#    #codesign -s "$APPCERT" -i $BUNDLEID $${TARGET}.app/Contents/Frameworks/QtCore.framework/Versions/4/QtCore
#    #codesign -s "$APPCERT" -i $BUNDLEID $${TARGET}.app/Contents/PlugIns/imageformats/libqjpeg.dylib

#    # Sign the application bundle, using the provided entitlements
#    codesign.commands += codesign -f -s $${APPCERT} -v –entitlements $${ENTITLEMENTS} $${TARGET}.app;

#    product.depends += all

#    # Build the product package
#    product.commands += productbuild –component $${TARGET}.app /Applications –sign $${INSTALLERCERT} $${TARGET}.pkg;

#    QMAKE_EXTRA_TARGETS += codesign product copyfiles