#include "mainwindow.h"
#include "coreapplication.h"
#include "version.h"

#include <QTranslator>

int main(int argc, char *argv[])
{
    Version::setVersion(Version(APP_VERSION), APP_DEPLOY, APP_DEBUG, GIT_CURRENT_SHA1);
    CoreApplication a(argc, argv);
    QObject::connect(&a, &CoreApplication::triggerOpen, MainWindow::open);

    QCoreApplication::setApplicationName("Einsatzplaner");

    QTranslator qtTranslator;
    qtTranslator.load(":/translations/qt_" + QLocale::system().name());
    a.installTranslator(&qtTranslator);

    QIcon icon(":/appIcon/Einsatzplaner.png");
    a.setWindowIcon(icon);

    if (a.generateWindow()) {
        MainWindow *w = new MainWindow();
        w->show();
    }
    int code = a.exec();
    a.stopAutoSave();
    return code;
}
