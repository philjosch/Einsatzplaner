CONFIG+=sdk_no_version_check


!versionAtLeast(QT_VERSION, 5.13.0):error("Qt version 5.13 is required for this project")

win32: LCONVERT=$$[QT_INSTALL_BINS]\lconvert.exe
else:  LCONVERT=$$[QT_INSTALL_BINS]/lconvert
system(\"$$LCONVERT\" -o \"$$_PRO_FILE_PWD_/qt_de.qm\" \"$$[QT_INSTALL_TRANSLATIONS]/qtbase_de.qm\")


QT       += core gui printsupport network widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


INCLUDEPATH += ../epl_api
LIBS += -L../epl_api -lepl_api
INCLUDEPATH += ../epl_gui
LIBS += -L../epl_gui -lepl_gui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


CONFIG -= console
CONFIG += macos
#OTHER_FILES += qt_de.qm

VERSION = 1.6.3
win32 {
    QMAKE_TARGET_COMPANY = Philipp Schepper
    QMAKE_TARGET_COPYRIGHT = Entwicklung von Philipp Schepper 2016-2020
    VERSION = 1.6.3.1
}


win32: COMMIT_HASH = $$system(git.exe -C \""$$_PRO_FILE_PWD_"\" rev-parse --short HEAD)
else:  COMMIT_HASH = $$system(git -C \""$$_PRO_FILE_PWD_"\" rev-parse --short HEAD)
DEFINES += GIT_CURRENT_SHA1="\"\\\"$${COMMIT_HASH}\\\"\""


RESOURCES += \
    ../resources.qrc
