TEMPLATE = aux

INSTALLER = Installer

INPUT = $$PWD/config/config.xml $$PWD/packages
example.input = INPUT
example.output = $$INSTALLER
example.commands = $$[QT_INSTALL_LIBS]/../../../Tools/QtInstallerFramework/3.1/bin/binarycreator -c $$PWD/config/config.xml -p $$PWD/packages EPL-Installer
example.CONFIG += target_predeps no_link combine

QMAKE_EXTRA_COMPILERS += example
OTHER_FILES += packages/de.philipp-schepper.epl/meta/*
#OTHER_FILES += packages/de.philipp-schepper.epl.online/meta/*
OTHER_FILES += README.md
