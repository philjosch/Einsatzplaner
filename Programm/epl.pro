#-------------------------------------------------
#
# Developers:
# 2016-2020: Philipp Schepper
#
#-------------------------------------------------

!versionAtLeast(QT_VERSION, 5.13.0):error("Qt version 5.13 is required for this project")

TEMPLATE = subdirs
SUBDIRS += Einsatzplaner \
    Personalplaner \
    epl_api \
    epl_gui

Einsatzplaner.depends = epl_api
Personalplaner.depends = epl_api


DISTFILES += share.pri
RESOURCES += resources.qrc
