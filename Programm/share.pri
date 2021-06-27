#-------------------------------------------------
#
# Developers:
# 2016-2021: Philipp Schepper
#
#-------------------------------------------------

###########
# VERSION #
###########
VERSION = 1.7.2
DEPLOYED = false

####################
# QT EINSTELLUNGEN #
####################
!versionAtLeast(QT_VERSION, 5.13.0):error("Qt version 5.13 is required for this project")

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


###########
# PROJEKT #
###########
TEMPLATE = app
QT       += core gui printsupport network widgets

CONFIG += c++11
CONFIG -= console

macx {
    CONFIG += macos
}


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


#################
# LOKALISIERUNG #
#################
win32: LCONVERT=$$[QT_INSTALL_BINS]\lconvert.exe
else:  LCONVERT=$$[QT_INSTALL_BINS]/lconvert
system(\"$$LCONVERT\" -o \"$$_PRO_FILE_PWD_/qt_de.qm\" \"$$[QT_INSTALL_TRANSLATIONS]/qtbase_de.qm\")

#OTHER_FILES += qt_de.qm


#############
# LIBRARIES #
#############
INCLUDEPATH += ../crypto ../epl_api ../epl_gui
LIBS += -L../crypto -lcrypto -L../epl_api -lepl_api -L../epl_gui -lepl_gui

DEPENDPATH +=      $$PWD/crypto $$PWD/epl_api $$PWD/epl_gui
PRE_TARGETDEPS +=  $$PWD/crypto $$PWD/epl_api $$PWD/epl_gui


#############
# PLATTFORM #
#############
win32 {
    CONFIG -= debug_and_release debug_and_release_target

    QMAKE_TARGET_DESCRIPTION = Ein Programm fuer Museumseisenbahnen
    QMAKE_TARGET_COMPANY = Philipp Schepper
    QMAKE_TARGET_COPYRIGHT = Entwicklung von Philipp Schepper 2016-2021

    QMAKE_TARGET_PRODUCT = $${TARGET}
    RC_ICONS = $$PWD/../Icon/$${TARGET}.ico
    VERSION = $${VERSION}.1
}
macx {
    QMAKE_INFO_PLIST = $$PWD/Info.plist
    ADDITIONAL_FILES.files = $$PWD/../Icon/EplDocIcon.icns $$PWD/../LICENSE
    ADDITIONAL_FILES.path = Contents/Resources
    QMAKE_BUNDLE_DATA += ADDITIONAL_FILES

    BUNDLEID = de.philipp-schepper.$${TARGET_KLEIN}
    ICON = $$PWD/../Icon/$${TARGET}.icns
}


#############
# RESOURCEN #
#############
RESOURCES += \
    $$PWD/icons/icons_rc.qrc \
    $$PWD/../Icon/appIcon.qrc \
    $$PWD/resources.qrc


###############
# TEMPORAERES #
###############
macx {
    CONFIG += sdk_no_version_check
}


#################
# VERSIONSDATEN #
#################
win32: COMMIT_HASH = $$system(git.exe -C \""$$_PRO_FILE_PWD_"\" rev-parse --short HEAD)
else:  COMMIT_HASH = $$system(git -C \""$$_PRO_FILE_PWD_"\" rev-parse --short HEAD)
DEFINES += GIT_CURRENT_SHA1="\"\\\"$${COMMIT_HASH}\\\"\""
DEFINES += APP_NAME="\"\\\"$${TARGET}\\\"\""
DEFINES += APP_VERSION="\"\\\"$${VERSION}\\\"\""
DEFINES += APP_DEPLOY="$${DEPLOYED}"

CONFIG(debug, debug|release) {
    DEFINES += APP_DEBUG="true"
}
CONFIG(release, debug|release) {
    DEFINES += APP_DEBUG="false"
}
