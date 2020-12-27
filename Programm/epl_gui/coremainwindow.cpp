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

    windows = QSet<QMainWindow*>();

    setWindowTitle(tr("Neues Dokument – Übersicht"));
    setWindowModified(false);
    setWindowIcon(QApplication::windowIcon());
}

bool CoreMainWindow::ladeDatei(QString path, QJsonObject json, bool schreibschutz)
{
    istSchreibgeschuetzt = schreibschutz;
    filePath = path;

    setWindowFilePath(filePath);
    // Update WindowFilePaths TODO::TODO
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
        foreach (QString entry, list) {
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
        setWindowModified(false);
        if (filePath != "") {
            QFile file (filePath+".autosave.ako");
            file.remove();
        }
        FileIO::History::insert(filePath);

        handlerSaveAdditional();
    } else {
        QMessageBox::warning(this, tr("Fehler"), tr("Das Speichern unter der angegebenen Adresse ist fehlgeschlagen!"));
    }
}
void CoreMainWindow::on_actionSaveas_triggered()
{
    QString newPath = FileIO::getFilePathSave(this, tr("Einsatzplan.ako"), tr("AkO-Dateien (*.ako)"));
    if (newPath != "") {
        if (!FileIO::Schreibschutz::pruefen(newPath).isEmpty()) {
            QMessageBox::warning(this, tr("Datei geöffnet"), tr("Unter der angegebenen Adresse befindet sich eine Datei, die aktuell geöffnet und bearbeitet wird. Ein Speichern ist somit nicht möglich!"));
            return;
        }

        filePath = newPath;
        QJsonObject o;
        bool erfolg = FileIO::saveJsonToFile(filePath, o);
        if (! erfolg) {
            QMessageBox::information(this, tr("Fehler"), tr("Das Speichern unter der angegebenen Adresse ist fehlgeschlagen!"));
            filePath = "";
            return;
        }
        istSchreibgeschuetzt = false;
        FileIO::Schreibschutz::setzen(filePath);
        setWindowTitle(tr("Übersicht"));
        setWindowFilePath(filePath);
        for(QMainWindow *mw: windows.values()) {
            mw->setWindowFilePath(filePath);
        }
        on_actionSave_triggered();
    }
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
    if (!FileIO::Schreibschutz::pruefen(path).isEmpty()) {
        QMessageBox::warning(this, tr("Datei geöffnet"), tr("Unter der angegebenen Adresse befindet sich eine Datei, die aktuell geöffnet und bearbeitet wird. Ein Speichern ist somit nicht möglich!"));
        return;
    }

    QJsonObject object = handlerSavePersonal();

    if (! FileIO::saveJsonToFile(path, object)) {
        QMessageBox::warning(this, tr("Fehler"), tr("Das Speichern unter der angegebenen Adresse ist fehlgeschlagen!"));
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
        } else if (answ == QMessageBox::Cancel) {
            toClose = false;
        } else if (answ == QMessageBox::Close) {
            toClose = close();
        }
    } else {
        toClose = true;
    }

    if (toClose) {
        for(QMainWindow *m: windows.values()) {
            m->close();
        }
        toClose = handlerClose();
        if (filePath != "") {
            QFile file (filePath+".autosave.ako");
            file.remove();
        }
    }

    if (toClose) {
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
    for(QMainWindow *m: windows.values()) {
        m->setWindowModified(true);
    }
}

bool CoreMainWindow::pruefeVersionMitWarnung(Version test)
{
    if (test > (CoreApplication::VERSION) || Version{-1,-1,-1} == test) {
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
