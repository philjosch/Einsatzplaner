#include "mainwindow.h"
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


void checkVersion();
QString loadVersion();

QString AKTUELLE_VERSION = "1.0.0";

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Laden der Einstellungen
    QCoreApplication::setOrganizationName("Philipp Schepper");
    QCoreApplication::setOrganizationDomain("philipp-schepper.de");
    QCoreApplication::setApplicationName("Einsatzplaner");
    QCoreApplication::setApplicationVersion(AKTUELLE_VERSION);

    QSettings settings;
    settings.setValue("test/bla", true);

    if (settings.value("general/autosearchupdate").toBool()) {
        checkVersion();
    }

    MainWindow w;
    w.show();

    return a.exec();
}


void checkVersion()
{
    QUrl url("Http://bahn.philipp-schepper.de/#downloads");
    QString v = loadVersion();
    QStringList online = v.split(".");
    QStringList aktuell = AKTUELLE_VERSION.split(".");

    if ((online.at(0) > aktuell.at(0)) || ((online.at(0) == aktuell.at(0)) && online.at(1) > aktuell.at(1)) ||
            ((online.at(0) == aktuell.at(0)) && (online.at(1) == aktuell.at(1)) && (online.at(2) > aktuell.at(2)))) {
        if (QMessageBox::information(nullptr, "Neue Version", "Es ist Version "+v+" des Programms verfügbar.\nSie benutzen Version "+AKTUELLE_VERSION+". ", QMessageBox::Open, QMessageBox::Ok) == QMessageBox::Open) {
            // Öffnen der Webseite
            QDesktopServices::openUrl(url);
        }

        qDebug() << "Bitte aktualisiern";
    }

}

QString loadVersion()
{
    QUrl url = QUrl("http://bahn.philipp-schepper.de/version.txt");

    // create custom temporary event loop on stack
    QEventLoop eventLoop;

    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    // the HTTP request
    QNetworkRequest req( url );
    QNetworkReply *reply = mgr.get(req);
    eventLoop.exec(); // blocks stack until "finished()" has been called

    if (reply->error() == QNetworkReply::NoError) {
        //success
        QString s = QString(reply->readAll());
        delete reply;
        return s;
    } else {
        //failure
        delete reply;
        return "";
    }
}
