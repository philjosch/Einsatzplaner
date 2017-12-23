#include "mainwindow.h"
#include "fileio.h"
#include "coreapplication.h"

#include <QApplication>
#include <QVariant>
#include <QSettings>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <QFileOpenEvent>
#include <QMenuBar>
#include <QMenu>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>

CoreApplication::Version AKTUELLE_VERSION = {1, 3, 0};

int main(int argc, char *argv[])
{
    CoreApplication a(argc, argv, AKTUELLE_VERSION);

    QTranslator qtTranslator;
    qtTranslator.load(":/translations/qt_" + QLocale::system().name());
    a.installTranslator(&qtTranslator);

    QTranslator myappTranslator;
    myappTranslator.load(":translations/einsatzplaner_" + QLocale::system().name());
    a.installTranslator(&myappTranslator);

    // Laden der Einstellungen
    FileIO::loadSettings();

    QSettings settings;

    if (settings.value("general/autosearchupdate", true).toBool()) {
        a.checkVersion();
    }
    if (a.isFirst == true) {
        MainWindow *w = new MainWindow();
        w->show();
    }
    return a.exec();
}

