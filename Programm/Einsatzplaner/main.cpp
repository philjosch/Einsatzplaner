#include "mainwindow.h"
#include "coreapplication.h"
#include "version.h"

#include <QTranslator>

int main(int argc, char *argv[])
{
    Version::setProgrammVersion(Version(APP_VERSION), APP_DEPLOY, APP_DEBUG, GIT_CURRENT_SHA1);
    CoreApplication a(argc, argv);
    QObject::connect(&a, &CoreApplication::triggerOpen, MainWindow::open);

    QCoreApplication::setApplicationName(APP_NAME);

    QTranslator qtTranslator;
    qtTranslator.load(QString(":/translations/qt_%1.qm").arg(QLocale::system().name()));
    a.installTranslator(&qtTranslator);

    QIcon icon(QString(":/appIcon/%1.icns").arg(APP_NAME));
    a.setWindowIcon(icon);

    if (a.generateWindow()) {
        CoreMainWindow *w = new MainWindow();
        w->show();
    }
    int code = a.exec();
    a.stopAutoSave();
    return code;
}
