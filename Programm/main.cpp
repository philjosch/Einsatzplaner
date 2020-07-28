#include "mainwindow.h"
#include "coreapplication.h"
#include "einstellungen.h"

#include <QSettings>
#include <QTranslator>

int main(int argc, char *argv[])
{
    CoreApplication a(argc, argv, {1, 6, 1}, true, GIT_CURRENT_SHA1);

    QTranslator qtTranslator;
    qtTranslator.load(":/translations/qt_" + QLocale::system().name());
    a.installTranslator(&qtTranslator);


    if (Einstellungen::getAutoSearchUpdate())
        a.checkVersion();

    if (int delay = Einstellungen::getAutoSave())
        a.startAutoSave(delay);

    if (a.getIsFirst() == true) {
        MainWindow *w = new MainWindow();
        w->show();
    }
    int code = a.exec();
    a.stopAutoSave();
    return code;
}
