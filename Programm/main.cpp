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

QString AKTUELLE_VERSION = "1.0.0";

int main(int argc, char *argv[])
{
    CoreApplication a(argc, argv, AKTUELLE_VERSION);

    // Laden der Einstellungen
    FileIO::loadSettings();

    QSettings settings;

    if (settings.value("general/autosearchupdate").toBool()) {
        a.checkVersion();
    }
    if (a.isFirst == true) {
        MainWindow *w = new MainWindow();
        w->show();
    }
    return a.exec();
}

