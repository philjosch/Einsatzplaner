#include "coreapplication.h"
#include "mainwindow.h"

#include <QDesktopServices>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMessageBox>
#include <QWindow>
#include <QTimer>

CoreApplication::Version CoreApplication::VERSION = {-1, -1, -1};
bool CoreApplication::DEVELOPER_MODE = false;
QUrl CoreApplication::URL_DOWNLOAD = QUrl("http://epl.philipp-schepper.de/#downloads");
QUrl CoreApplication::URL_VERSION = QUrl("http://epl.philipp-schepper.de/version.txt");
QString CoreApplication::URL_NOTES = "http://epl.philipp-schepper.de/version/";

CoreApplication::CoreApplication(int &argc, char **argv, CoreApplication::Version version, bool devVersion) : QApplication(argc, argv)
{
    VERSION = version;
    DEVELOPER_MODE = devVersion;

    QCoreApplication::setOrganizationName("Philipp Schepper");
    QCoreApplication::setOrganizationDomain("philipp-schepper.de");
    QCoreApplication::setApplicationName("Einsatzplaner");
    QCoreApplication::setApplicationVersion(VERSION.toString());
    QIcon icon(":/icons/square.png");
    setWindowIcon(icon);
    isFirst = true;
    autoSaveTimer = nullptr;
}

CoreApplication::~CoreApplication()
{

}

bool CoreApplication::getIsFirst() const
{
    return isFirst;
}

bool CoreApplication::event(QEvent *event)
{
    if (event->type() == QEvent::FileOpen) {
        isFirst = false;
        QFileOpenEvent *openEvent = static_cast<QFileOpenEvent *>(event);
        MainWindow::open(openEvent->file());
    }
    return QApplication::event(event);
}

void CoreApplication::checkVersion()
{
    Version v = loadVersion();
    if (v>VERSION) {
        QString message = tr("Es ist Version %1 des Programms verfügbar.\nSie benutzen Version %2.\n\n").arg(v.toString()).arg(VERSION.toString());
        QMessageBox::StandardButton answ = QMessageBox::information(nullptr, tr("Neue Version"), message, QMessageBox::Ignore|QMessageBox::Help|QMessageBox::Open, QMessageBox::Open);
        if (answ == QMessageBox::Open) {
            QDesktopServices::openUrl(URL_DOWNLOAD);
        } else if (answ == QMessageBox::Help) {
            if (QMessageBox::information(nullptr, tr("Über die neue Version"), loadNotes(v), QMessageBox::Close|QMessageBox::Open, QMessageBox::Open) == QMessageBox::Open) {
                QDesktopServices::openUrl(URL_DOWNLOAD);
            }
        }
    }
}

void CoreApplication::startAutoSave(int delay)
{
    autoSaveTimer = new QTimer();
    connect(autoSaveTimer, SIGNAL(timeout()), this, SLOT(autoSaveWindows()));
    if (delay <= 0) return;
    autoSaveTimer->start(delay*1000);
}
void CoreApplication::autoSaveWindows()
{
    foreach (QWidget *w, allWidgets()) {
        if (MainWindow *mW = dynamic_cast<MainWindow*>(w)) {
            mW->autoSave();
        }
    }
}
void CoreApplication::stopAutoSave()
{
    // Auto-Save Thread beenden, falls er gestartet wurde
    if (autoSaveTimer != nullptr) {
        autoSaveTimer->stop();
    }
}

CoreApplication::Version CoreApplication::loadVersion()
{
    // create custom temporary event loop on stack
    QEventLoop eventLoop;

    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    // the HTTP request
    QNetworkRequest req( URL_VERSION );
    QNetworkReply *reply = mgr.get(req);
    eventLoop.exec(); // blocks stack until "finished()" has been called

    if (reply->error() == QNetworkReply::NoError) {
        //success
        QString s = QString(reply->readAll());
        delete reply;
        return Version::stringToVersion(s);
    } else {
        //failure
        delete reply;
        return Version {-1, -1, -1};
    }
}

QString CoreApplication::loadNotes(Version v)
{

    QUrl url = QUrl(URL_NOTES + QString("v%1-%2/notes-v%1-%2-%3.txt").arg(v.major).arg(v.minor).arg(v.patch));

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

void CoreApplication::closeAllWindows()
{
    bool ok = true;
    QWindowList liste = allWindows();
    for (int i = 0; i < liste.length(); i++) {
        ok = ok && liste.at(i)->close();
    }
    if (ok) quit();
}
