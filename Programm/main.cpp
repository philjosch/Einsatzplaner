#include "mainwindow.h"
#include "fileio.h"
#include "coreapplication.h"

#include <QSettings>
#include <QTranslator>

int main(int argc, char *argv[])
{
    CoreApplication a(argc, argv, {1, 6, 0}, true);

    QTranslator qtTranslator;
    qtTranslator.load(":/translations/qt_" + QLocale::system().name());
    a.installTranslator(&qtTranslator);

    // Laden der Einstellungen
    FileIO::loadSettings();

    QSettings settings;

    if (settings.value("general/autosearchupdate", true).toBool())
        a.checkVersion();

    if (int delay = settings.value("io/autoSave", 0).toInt())
        a.startAutoSave(delay);

    if (a.getIsFirst() == true) {
        MainWindow *w = new MainWindow();
        w->show();
    }
    int code = a.exec();
    a.stopAutoSave();
    return code;
}
