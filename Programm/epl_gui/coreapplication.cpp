#include "coreapplication.h"
#include "coremainwindow.h"
#include "version.h"

#include <QDesktopServices>
#include <QMessageBox>
#include <QWindow>
#include <QTimer>
#include <QFileOpenEvent>
#include <QTranslator>
#include <einstellungen.h>
#include <QWindow>

QString CoreApplication::URL_DOWNLOAD = "http://epl.philipp-schepper.de/#downloads";

CoreApplication::CoreApplication(int &argc, char **argv) : QApplication(argc, argv)
{
    QCoreApplication::setOrganizationName("Philipp Schepper");
    QCoreApplication::setOrganizationDomain("philipp-schepper.de");

    autoSaveTimer = nullptr;

//    setAttribute(Qt::AA_UseHighDpiPixmaps);
}

CoreApplication::~CoreApplication()
{
    closeAllWindows();
}

CoreApplication* CoreApplication::generateApp(int argc, char *argv[], QString name, QString vers, bool deploy, bool debug, QString hash)
{
    Version::setProgrammVersion(Version(vers), deploy, debug, hash);
    CoreApplication *a = new CoreApplication(argc, argv);

    QCoreApplication::setApplicationName(name);

    QIcon icon(QString(":/appIcon/%1.icns").arg(name));
    a->setWindowIcon(icon);

//    QTranslator qtTranslator;
//    qtTranslator.load(lang);
//    a->installTranslator(&qtTranslator);

    if (Einstellungen::getAutoSearchUpdate())
        a->checkVersion();
    if (int delay = Einstellungen::getAutoSave())
        a->startAutoSave(delay);

    return a;
}

bool CoreApplication::generateWindow() const
{
    return (getCoreMainWindows().isEmpty());
}

bool CoreApplication::event(QEvent *event)
{
    if (event->type() == QEvent::FileOpen) {
        QFileOpenEvent *openEvent = static_cast<QFileOpenEvent *>(event);
        if (openHandlerSet)
            emit triggerOpen(openEvent->file());
        else {
            unopenedFiles.append(openEvent->file());
        }
    }
    return QApplication::event(event);
}

void CoreApplication::checkVersion() const
{
    if (Version::isUpdateVerfuegbar()) {
        Version v = Version::ladeNeusteVersion();
        QString message = tr("Es ist Version %1 des Programms verfügbar.\nSie benutzen Version %2.")
                .arg(v.toString(), QCoreApplication::applicationVersion());
        QMessageBox::StandardButton answ = QMessageBox::information(nullptr, tr("Neue Version"), message, QMessageBox::Ignore|QMessageBox::Help|QMessageBox::Open, QMessageBox::Open);
        if (answ == QMessageBox::Open) {
            oeffneDownloadSeite();
        } else if (answ == QMessageBox::Help) {
            if (QMessageBox::information(nullptr,
                                         tr("Über die neue Version"),
                                         Version::loadNotes(v),
                                         QMessageBox::Close|QMessageBox::Open,
                                         QMessageBox::Open) == QMessageBox::Open) {
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
    for(CoreMainWindow *w: getCoreMainWindows()) {
        w->autoSave();
    }
}

void CoreApplication::openUnopenedFiles()
{
    openHandlerSet = true;
    while(!unopenedFiles.isEmpty()) {
        emit triggerOpen(unopenedFiles.takeFirst());
    }
}

void CoreApplication::oeffneDownloadSeite()
{
    QDesktopServices::openUrl(URL_DOWNLOAD);
}

QList<CoreMainWindow *> CoreApplication::getCoreMainWindows()
{
    QList<CoreMainWindow*> l;
    for(QWidget *w: allWidgets()) {
        if (CoreMainWindow *mW = dynamic_cast<CoreMainWindow*>(w)) {
            l.append(mW);
        }
    }
    return l;
}

void CoreApplication::stopAutoSave() const
{
    // Auto-Save Thread beenden, falls er gestartet wurde
    if (autoSaveTimer != nullptr) {
        autoSaveTimer->stop();
    }
}
void CoreApplication::closeAllWindows()
{
    bool ok = true;
    QList<CoreMainWindow*> liste = getCoreMainWindows();
    for (int i = 0; i < liste.length(); i++) {
        ok = ok && liste.at(i)->close();
    }
    if (ok) quit();
}
