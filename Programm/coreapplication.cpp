#include "coreapplication.h"
#include "mainwindow.h"

#include <QApplication>
#include <QFileOpenEvent>
#include <QDesktopServices>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMessageBox>
#include <QWindow>


QString CoreApplication::aktuelleVersion = "0.0.0";
QUrl CoreApplication::urlDownload = QUrl("http://bahn.philipp-schepper.de/#downloads");
QUrl CoreApplication::urlVersion = QUrl("http://bahn.philipp-schepper.de/version_developer.txt");

CoreApplication::CoreApplication(int &argc, char **argv, QString version) : QApplication(argc, argv)
{
    aktuelleVersion = version;

    QCoreApplication::setOrganizationName("Philipp Schepper");
    QCoreApplication::setOrganizationDomain("philipp-schepper.de");
    QCoreApplication::setApplicationName("Einsatzplaner");
    QCoreApplication::setApplicationVersion(aktuelleVersion);
    QIcon icon(":/icons/square.png");
    setWindowIcon(icon);
    isFirst = true;
}

CoreApplication::~CoreApplication()
{

}

bool CoreApplication::event(QEvent *event)
{
    if (event->type() == QEvent::FileOpen) {
        isFirst = false;
        QFileOpenEvent *openEvent = static_cast<QFileOpenEvent *>(event);
        MainWindow *mw = new MainWindow();
        if (mw->openFile(openEvent->file())) {
            mw->show();
        } else {
            delete mw;
        }
    }
    return QApplication::event(event);
}

bool CoreApplication::versionGreater(QString firstV, QString secondV)
{
    QStringList first = firstV.split(".");
    QStringList second = secondV.split(".");

    if (first.at(0) > second.at(0)) {
        return true;
    }
    if ((first.at(0) == second.at(0)) && (first.at(1).toInt() > second.at(1).toInt())) {
        return true;
    }
    if ((first.at(0) == second.at(0)) && (first.at(1) == second.at(1)) && (first.at(2).toInt() > second.at(2).toInt())) {
        return true;
    }
    return false;
}

bool CoreApplication::versionGreater(QString version)
{
    return versionGreater(version, aktuelleVersion);
}

void CoreApplication::checkVersion()
{
    QString v = loadVersion();

    if (versionGreater(v)) {
        if (QMessageBox::information(nullptr, tr("Neue Version"), tr("Es ist Version ")+v+tr(" des Programms verfügbar.\nSie benutzen Version ")+aktuelleVersion+". ", QMessageBox::Ignore|QMessageBox::Open, QMessageBox::Open) == QMessageBox::Open) {
            QDesktopServices::openUrl(urlDownload);
        }
    }
}

QString CoreApplication::getAktuelleVersion()
{
    return aktuelleVersion;
}

void CoreApplication::closeAllWindows()
{
    bool ok = true;
    QWindowList liste = allWindows();
    for (int i = 0; i < liste.length(); i++) {
        ok = ok && liste.at(i)->close();
    }
    if (ok) quit();
}

QUrl CoreApplication::getUrlDownload()
{
    return urlDownload;
}

QString CoreApplication::loadVersion()
{
    // create custom temporary event loop on stack
    QEventLoop eventLoop;

    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    // the HTTP request
    QNetworkRequest req( urlVersion );
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
