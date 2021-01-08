#-------------------------------------------------
#
# Developers:
# 2016-2021: Philipp Schepper
#
#-------------------------------------------------

!versionAtLeast(QT_VERSION, 5.13.0):error("Qt version 5.13 is required for this project")

TEMPLATE = subdirs
SUBDIRS += Einsatzplaner \
    Personalplaner \
    epl_api \
    epl_gui

Einsatzplaner.depends = epl_api epl_gui
Personalplaner.depends = epl_api epl_gui


DISTFILES += share.pri
RESOURCES += resources.qrc
OTHER_FILES += Info.plist
