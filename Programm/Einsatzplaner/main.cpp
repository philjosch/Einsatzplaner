#include "mainwindowevents.h"
#include "coreapplication.h"

#include <QTranslator>

int main(int argc, char *argv[])
{
//    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    CoreApplication *a = CoreApplication::generateApp(argc, argv, APP_NAME, APP_VERSION, APP_DEPLOY, APP_DEBUG, GIT_CURRENT_SHA1);
    QObject::connect(a, &CoreApplication::triggerOpen, MainWindowEvents::open);

    QTranslator qtTranslator;
    if (qtTranslator.load(QString(":/translations/qt_%1.qm").arg(QLocale::system().name().split("_").at(0))))
        a->installTranslator(&qtTranslator);

    a->openUnopenedFiles();

    if (a->generateWindow()) {
        CoreMainWindow *w = new MainWindowEvents();
        w->show();
    }
    int code = a->exec();
    a->stopAutoSave();
    return code;
}
