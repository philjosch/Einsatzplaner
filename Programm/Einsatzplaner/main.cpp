#include "mainwindow.h"
#include "coreapplication.h"

#include <QTranslator>

int main(int argc, char *argv[])
{
    CoreApplication *a = CoreApplication::generateApp(argc, argv, APP_NAME, APP_VERSION, APP_DEPLOY, APP_DEBUG, GIT_CURRENT_SHA1);
    QObject::connect(a, &CoreApplication::triggerOpen, MainWindow::open);
    a->openUnopenedFiles();

    if (a->generateWindow()) {
        CoreMainWindow *w = new MainWindow();
        w->show();
    }
    int code = a->exec();
    a->stopAutoSave();
    return code;
}
