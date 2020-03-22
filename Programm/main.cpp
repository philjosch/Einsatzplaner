#include "mainwindow.h"
#include "fileio.h"
#include "coreapplication.h"

#include <QSettings>
#include <QTranslator>

const CoreApplication::Version AKTUELLE_VERSION = {1, 5, 2};
const bool IS_DEVELOPER_VERSION = false;

int main(int argc, char *argv[])
{
    CoreApplication a(argc, argv, AKTUELLE_VERSION, IS_DEVELOPER_VERSION);

    QTranslator qtTranslator;
    qtTranslator.load(":/translations/qt_" + QLocale::system().name());
    a.installTranslator(&qtTranslator);

    QTranslator myappTranslator;
    myappTranslator.load(":/translations/einsatzplaner_" + QLocale::system().name());
    a.installTranslator(&myappTranslator);

    // Laden der Einstellungen
    FileIO::loadSettings();

    QSettings settings;

    if (settings.value("general/autosearchupdate", true).toBool())
        a.checkVersion();

    if (int delay = settings.value("io/autoSave", 0).toInt())
        a.startAutoSave(delay*60); // Funktion nimmt Wert in Sekunden!

    if (a.getIsFirst() == true) {
        MainWindow *w = new MainWindow();
        w->show();
    }
    int code = a.exec();
    a.stopAutoSave();
    return code;
}

