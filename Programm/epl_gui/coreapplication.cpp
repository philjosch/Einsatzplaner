#include "coreapplication.h"
#include "coremainwindow.h"
#include "networking.h"

#include <QDesktopServices>
#include <QMessageBox>
#include <QWindow>
#include <QTimer>
#include <QFileOpenEvent>
#include <QTranslator>
#include <einstellungen.h>
#include <QWindow>

Version CoreApplication::VERSION = {-1, -1, -1};
QString CoreApplication::BUILD = "";
bool CoreApplication::DEVELOPER_MODE = false;
QString CoreApplication::URL_DOWNLOAD = "http://epl.philipp-schepper.de/#downloads";
QString CoreApplication::URL_VERSION = "http://epl.philipp-schepper.de/version.txt";
QString CoreApplication::URL_NOTES = "http://epl.philipp-schepper.de/version/v%1-%2/notes-v%1-%2-%3.txt";

CoreApplication::CoreApplication(int &argc, char **argv, Version version, bool devVersion, QString build) : QApplication(argc, argv)
{
    VERSION = version;
    DEVELOPER_MODE = devVersion;
    BUILD = build;

    QCoreApplication::setOrganizationName("Philipp Schepper");
    QCoreApplication::setOrganizationDomain("philipp-schepper.de");
    QCoreApplication::setApplicationName("Einsatzplaner");
    if (DEVELOPER_MODE) {
        QCoreApplication::setApplicationVersion(QString("%1 (%2)").arg(VERSION.toString()).arg(CoreApplication::BUILD));
    } else {
        QCoreApplication::setApplicationVersion(VERSION.toString());
    }
    QIcon icon(":/icons/square.png");
    setWindowIcon(icon);
    autoSaveTimer = nullptr;

    QTranslator qtTranslator;
    qtTranslator.load(":/translations/qt_" + QLocale::system().name());
    installTranslator(&qtTranslator);


    if (Einstellungen::getAutoSearchUpdate())
        checkVersion();

    if (int delay = Einstellungen::getAutoSave())
        startAutoSave(delay);
}

CoreApplication::~CoreApplication()
{

}

bool CoreApplication::generateWindow()
{
    for (QWidget *w: allWidgets()) {
        if (CoreMainWindow *mW = dynamic_cast<CoreMainWindow*>(w)) {
            return false;
        }
    }
    return true;
}

bool CoreApplication::event(QEvent *event)
{
    if (event->type() == QEvent::FileOpen) {
        QFileOpenEvent *openEvent = static_cast<QFileOpenEvent *>(event);
        emit triggerOpen(openEvent->file());
    }
    return QApplication::event(event);
}

void CoreApplication::checkVersion()
{
    if (isUpdateVerfuegbar()) {
        Version v = loadVersion();
        QString message = tr("Es ist Version %1 des Programms verfügbar.\nSie benutzen Version %2.")
                .arg(v.toString()).arg(QCoreApplication::applicationVersion());
        QMessageBox::StandardButton answ = QMessageBox::information(nullptr, tr("Neue Version"), message, QMessageBox::Ignore|QMessageBox::Help|QMessageBox::Open, QMessageBox::Open);
        if (answ == QMessageBox::Open) {
            oeffneDownloadSeite();
        } else if (answ == QMessageBox::Help) {
            if (QMessageBox::information(nullptr, tr("Über die neue Version"), loadNotes(v), QMessageBox::Close|QMessageBox::Open, QMessageBox::Open) == QMessageBox::Open) {
                oeffneDownloadSeite();
            }
        }
    }
}

void CoreApplication::startAutoSave(int delay)
{
    autoSaveTimer = new QTimer();
    connect(autoSaveTimer, SIGNAL(timeout()), this, SLOT(autoSaveWindows()));
    if (delay <= 0) return;
    autoSaveTimer->start(delay*60000);
}
void CoreApplication::autoSaveWindows()
{
    for(QWindow *w: allWindows()) {
        if (CoreMainWindow *mW = dynamic_cast<CoreMainWindow*>(w)) {
            mW->autoSave();
        }
    }
}

void CoreApplication::oeffneDownloadSeite()
{
    QDesktopServices::openUrl(URL_DOWNLOAD);
}

void CoreApplication::stopAutoSave()
{
    // Auto-Save Thread beenden, falls er gestartet wurde
    if (autoSaveTimer != nullptr) {
        autoSaveTimer->stop();
    }
}

Version CoreApplication::loadVersion()
{
    QString s = Networking::ladeDatenVonURL(URL_VERSION);
    if (s == "") {
        return Version {-1, -1, -1};
    } else {
        return Version::stringToVersion(s);
    }
}

QString CoreApplication::loadNotes(Version v)
{
    return Networking::ladeDatenVonURL(URL_NOTES.arg(v.major).arg(v.minor).arg(v.patch));
}

bool CoreApplication::isUpdateVerfuegbar()
{
    Version v = loadVersion();
    return (v>VERSION) || ((v == VERSION) && DEVELOPER_MODE);
}

[[deprecated("Moved to basics")]]
bool CoreApplication::isSupportedVersion(Version test)
{
    if (test == Version{-1,-1,-1}) return false;
    if (test > CoreApplication::VERSION) return false;
    return true;
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
