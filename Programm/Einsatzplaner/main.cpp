#include "mainwindow.h"
#include "coreapplication.h"

#include <QTranslator>

int main(int argc, char *argv[])
{
    CoreApplication *a = CoreApplication::generateApp(argc, argv, APP_NAME, APP_VERSION, APP_DEPLOY, APP_DEBUG, GIT_CURRENT_SHA1);
    QObject::connect(a, &CoreApplication::triggerOpen, MainWindow::open);
    a->openUnopenedFiles();

    QTranslator qtTranslator;
    qtTranslator.load(QString(":/translations/qt_%1.qm").arg(QLocale::system().name().split("_").at(0)));
    a->installTranslator(&qtTranslator);

    if (a->generateWindow()) {
        CoreMainWindow *w = new MainWindow();
        w->show();
    }
    int code = a->exec();
    a->stopAutoSave();
    return code;
}
