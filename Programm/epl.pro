#-------------------------------------------------
#
# Developers:
# 2016-2022: Philipp Schepper
#
#-------------------------------------------------

TEMPLATE = subdirs
SUBDIRS += Einsatzplaner \
    Personalplaner \
    epl_api \
    epl_gui \
    crypto

Einsatzplaner.depends = crypto epl_api epl_gui
Personalplaner.depends = crypto epl_api epl_gui


RESOURCES += $$PWD/resources.qrc \
    $$PWD/icons/icons_rc.qrc \
    $$PWD/../Icon/appicon.qrc

OTHER_FILES += Info.plist

win32 {
    CONFIG -= debug_and_release debug_and_release_target
}
