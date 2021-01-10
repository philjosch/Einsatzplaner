#include "mainwindow.h"
#include "coreapplication.h"

#include <QTranslator>

int main(int argc, char *argv[])
{
    CoreApplication a(argc, argv, Version::CURRENT_API_VERSION, true, GIT_CURRENT_SHA1);
    QObject::connect(&a, &CoreApplication::triggerOpen, MainWindow::open);

    QCoreApplication::setApplicationName("Einsatzplaner");

    QTranslator qtTranslator;
    qtTranslator.load(":/translations/qt_" + QLocale::system().name());
    a.installTranslator(&qtTranslator);

    QIcon icon(":/appIcon/EinsatzplanerAppIcon.png");
    a.setWindowIcon(icon);

    if (a.generateWindow()) {
        MainWindow *w = new MainWindow();
        w->show();
    }
    int code = a.exec();
    a.stopAutoSave();
    return code;
}
