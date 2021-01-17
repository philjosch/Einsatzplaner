#-------------------------------------------------
#
# Developers:
# 2016-2021: Philipp Schepper
#
#-------------------------------------------------

TEMPLATE = subdirs
SUBDIRS += Einsatzplaner \
    Personalplaner \
    epl_api \
    epl_gui

Einsatzplaner.depends = epl_api epl_gui
Personalplaner.depends = epl_api epl_gui


RESOURCES += $$PWD/resources.qrc \
    $$PWD/icons/icons_rc.qrc \
    $$PWD/../Icon/appicon.qrc

OTHER_FILES += Info.plist
