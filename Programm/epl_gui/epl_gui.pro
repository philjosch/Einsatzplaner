#-------------------------------------------------
#
# Developers:
# 2016-2021: Philipp Schepper
#
#-------------------------------------------------
include(../share.pri)

TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    guihelper.cpp \
    coremainwindow.cpp \
    einstellungendialog.cpp \
    minimumhourseditordialog.cpp \
    coreapplication.cpp

HEADERS += \
    guihelper.h \
    coremainwindow.h \
    einstellungendialog.h \
    minimumhourseditordialog.h \
    coreapplication.h

FORMS += \
    minimumhourseditordialog.ui \
    einstellungendialog.ui

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target
