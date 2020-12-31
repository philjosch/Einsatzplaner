#include "mainwindow.h"
#include "coreapplication.h"

int main(int argc, char *argv[])
{
    CoreApplication a(argc, argv, {1, 6, 3}, true, GIT_CURRENT_SHA1);

    QObject::connect(&a, &CoreApplication::triggerOpen, MainWindow::open);

    if (a.generateWindow()) {
        MainWindow *w = new MainWindow();
        w->show();
    }
    int code = a.exec();
    a.stopAutoSave();
    return code;
}
