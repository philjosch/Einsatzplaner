#include "mainwindowpersonal.h"
#include "coreapplication.h"
#include "version.h"

#include <QTranslator>

int main(int argc, char *argv[])
{
    Version::setVersion(Version(APP_VERSION), APP_DEPLOY, APP_DEBUG, GIT_CURRENT_SHA1);
    CoreApplication a(argc, argv);
    QObject::connect(&a, &CoreApplication::triggerOpen, MainWindowPersonal::open);

    QCoreApplication::setApplicationName("Personalplaner");

    QTranslator qtTranslator;
    qtTranslator.load(":/translations/qt_" + QLocale::system().name());
    a.installTranslator(&qtTranslator);

    QIcon icon(":/appIcon/Personalplaner.png");
    a.setWindowIcon(icon);

    if (a.generateWindow()) {
        MainWindowPersonal *w = new MainWindowPersonal();
        w->show();
    }
    int code = a.exec();
    a.stopAutoSave();
    return code;
}
