#include "coremainwindow.h"
#include "export.h"
#include "fileio.h"
#include "coreapplication.h"

#include <QMessageBox>
#include <QJsonArray>
#include <QMessageBox>

CoreMainWindow::CoreMainWindow(QWidget *parent) : QMainWindow(parent)
{
    filePath = "";
    istSchreibgeschuetzt = false;
    saved = true;
    // Views

    setWindowTitle(tr("Übersicht"));
    updateWindowHeaders(false);
    setWindowIcon(QApplication::windowIcon());
}

bool CoreMainWindow::ladeDatei(QString path, QJsonObject json, bool schreibschutz)
{
    istSchreibgeschuetzt = schreibschutz;
    filePath = path;

    updateWindowHeaders(false, filePath);
    handlerLadeDatei(json);
    return true;
}

CoreMainWindow::~CoreMainWindow()
{
    close();
}

void CoreMainWindow::on_actionPreferences_triggered()
{
    handlerPreferenes();
}
void CoreMainWindow::on_actionAboutQt_triggered()
{
    QMessageBox::aboutQt(this);
}
void CoreMainWindow::on_actionAboutApp_triggered()
{
    QMessageBox::about(this,
                       tr("Über Einsatzplaner"),
                       tr("<h1>Einsatzplaner</h1><p>Version %1<br/>2016-2020 Philipp Schepper</p>")
                       .arg(QCoreApplication::applicationVersion()));
}
void CoreMainWindow::on_actionQuit_triggered()
{
    CoreApplication::closeAllWindows();
}


void CoreMainWindow::on_actionNew_triggered()
{
    CoreMainWindow *mw = handlerNew();
    if (mw != nullptr) mw->show();
}

void CoreMainWindow::on_actionOpen_triggered()
{
    handlerOpen(FileIO::getFilePathOpen(this, tr("AkO-Dateien (*.ako)")));
}

void CoreMainWindow::on_menuRecentlyused_aboutToShow()
{
    QStringList list = FileIO::History::get();

    if (list.length() > 0) {
        QList<QAction*> actions = recentlyUsedMenu->actions();
        for (int i = 2; i < actions.length(); i++) {
            recentlyUsedMenu->removeAction(actions[i]);
        }
        recentlyUsedClear->setEnabled(true);
        for (QString entry: list) {
            QAction *a = new QAction(entry, this);
            connect(a, &QAction::triggered, this, [=]() { handlerOpen(entry); });
            recentlyUsedMenu->insertAction(nullptr, a);
        }
    }
}
void CoreMainWindow::on_actionClear_triggered()
{
    FileIO::History::clear();
    QList<QAction*> actions = recentlyUsedMenu->actions();
    for (int i = 2; i < actions.length(); i++) {
        recentlyUsedMenu->removeAction(actions[i]);
    }
    recentlyUsedClear->setEnabled(false);
}

void CoreMainWindow::on_actionSave_triggered()
{
    if (istSchreibgeschuetzt) {
        QMessageBox::information(this, tr("Nicht gespeichert"), tr("Die Datei konnte nicht gespeichert werden, da sie schreibgeschützt geöffnet wurde."));
        return;
    }
    if (filePath == "") {
        on_actionSaveas_triggered();
        return;
    }

    bool erfolg = saveToPath(filePath);
    if (erfolg) {
        saved = true;
        updateWindowHeaders(false);
        if (filePath != "") {
            QFile file (filePath+".autosave.ako");
            file.remove();
        }
        FileIO::History::insert(filePath);

        handlerSaveAdditional();
    } else {
        saveFailed();
    }
}
void CoreMainWindow::on_actionSaveas_triggered()
{
    QString newPath = FileIO::getFilePathSave(this, tr("Einsatzplan.ako"), tr("AkO-Dateien (*.ako)"));
    if (newPath == "") return;

    QJsonObject o;
    bool erfolg = FileIO::saveJsonToFile(newPath, o);
    if (! erfolg) {
        saveFailed();
        return;
    }
    filePath = newPath;
    istSchreibgeschuetzt = false;
    FileIO::Schreibschutz::setzen(filePath);
    setWindowTitle(tr("Übersicht"));
    updateWindowHeaders(false, filePath);
    on_actionSave_triggered();
}
void CoreMainWindow::autoSave()
{
    if (istSchreibgeschuetzt) return;
    if (filePath == "") return;
    if (saved) return;
    saveToPath(filePath+".autosave.ako");
}
bool CoreMainWindow::saveToPath(QString path)
{
    if (istSchreibgeschuetzt) return false;
    QJsonObject json = handlerSave();
    return FileIO::saveJsonToFile(path, json);
}

void CoreMainWindow::on_actionSavePersonal_triggered()
{
    QString path = FileIO::getFilePathSave(this, tr("Einsatzplan.ako"), tr("AkO-Dateien (*.ako)"));
    if (path == "") return;

    QJsonObject object = handlerSavePersonal();

    if (! FileIO::saveJsonToFile(path, object)) {
        saveFailed();
    }
}

void CoreMainWindow::on_actionSettings_triggered()
{
    handlerSettings();
}

bool CoreMainWindow::on_actionClose_triggered()
{
    return close();
}
void CoreMainWindow::closeEvent(QCloseEvent *event)
{
    bool toClose = false;
    if (!saved) {
        QMessageBox::StandardButton answ = QMessageBox::question(this, tr("Ungesicherte Änderungen"),
                                                                 tr("Möchten Sie die Datei wirklich schließen?\nIhre ungesicherten Änderungen gehen dann verloren!"),
                                                                 QMessageBox::Close|QMessageBox::Cancel|QMessageBox::Save, QMessageBox::Save);
        if (answ == QMessageBox::Save) {
            on_actionSave_triggered();
            if (saved) {
                toClose = close();
            }
        } else if (answ == QMessageBox::Close) {
            toClose = close();
        }
    } else {
        toClose = true;
    }

    if (toClose) {
        for(QMainWindow *w: getChildWindows()) {
            w->close();
        }
        toClose = handlerClose();
    }
    if (toClose) {
        if (filePath != "") {
            QFile file (filePath+".autosave.ako");
            file.remove();
        }
        if (!istSchreibgeschuetzt)
            FileIO::Schreibschutz::freigeben(filePath);
        event->accept();
    } else {
        event->ignore();
    }
}

void CoreMainWindow::unsave()
{
    saved = false;
    updateWindowHeaders(true);
}

bool CoreMainWindow::pruefeVersionMitWarnung(Version test)
{
    if (!CoreApplication::isSupportedVersion(test)) {
        QMessageBox::warning(nullptr, tr("Nicht kompatibel"),
                             tr("Die Datei kann nicht mit dieser Version geöffnet werden. "
                                "Das Dokument benötigt mindestens Version %1.\n"
                                "Die aktuellste Version finden Sie auf der Webseite des Programms.").arg(test.toStringShort()));
        return false;
    }
    return true;
}

bool CoreMainWindow::setzeSchreibschutzOderWarnung(QString pfad)
{
    QStringList l = FileIO::Schreibschutz::pruefen(pfad);
    if (!l.isEmpty()) {
        QMessageBox::information(nullptr,
                                 tr("Schreibgeschützt"),
                                 tr("Die ausgewählte Datei wurde am %1 von \"%2\" zuletzt geöffnet und wird seitdem bearbeitet. Die Datei wird deshalb schreibgeschützt geöffnet!").arg(l.at(0),l.at(1)));
        return false;
    } else {
        FileIO::Schreibschutz::setzen(pfad);
        return true;
    }
}

QList<QMainWindow *> CoreMainWindow::getChildWindows()
{
    QList<QMainWindow*> list;
    for(QObject *w: children()) {
        if (QMainWindow *mW = dynamic_cast<QMainWindow*>(w)) {
            list.append(mW);
        }
    }
    return list;
}

void CoreMainWindow::updateWindowHeaders(bool modified, QString path)
{
    QList<QMainWindow*> list = getChildWindows();
    for(QMainWindow *m: list) {
        m->setWindowModified(modified);
    }

    if (path != "") {
        setWindowFilePath(path);
        for(QMainWindow *mw: list) {
            mw->setWindowFilePath(path);
        }
    }
}

void CoreMainWindow::saveFailed()
{
    QMessageBox::warning(this, tr("Fehler"),
                         tr("Das Speichern unter der angegebenen Adresse ist fehlgeschlagen!"));
}
