#include "coremainwindow.h"
#include "export.h"
#include "fileio.h"
#include "coreapplication.h"
#include "einstellungendialog.h"

#include <QMessageBox>

CoreMainWindow::CoreMainWindow(QWidget *parent) : QMainWindow(parent)
{
    datei = new EplFile();
    connect(datei, &EplFile::changed, this, &CoreMainWindow::unsave);
    // Views

    setWindowTitle(tr("Übersicht"));
    updateWindowHeaders();
    setWindowIcon(QApplication::windowIcon());
    setAttribute(Qt::WA_DeleteOnClose, true);
}

CoreMainWindow::CoreMainWindow(EplFile *datei, QWidget *parent) : QMainWindow(parent)
{
    this->datei = datei;
    connect(datei, &EplFile::changed, this, &CoreMainWindow::unsave);
    // Views

    setWindowTitle(tr("Übersicht"));
    updateWindowHeaders();
    setWindowIcon(QApplication::windowIcon());
    setAttribute(Qt::WA_DeleteOnClose, true);
}

CoreMainWindow::~CoreMainWindow()
{
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
                       tr("Über %1").arg(QCoreApplication::applicationName()),
                       tr("<h1>%1</h1><p>Ein Programm der EPL-Familie<br/>Version %2<br/>2016-2021 Philipp Schepper</p>")
                       .arg(QCoreApplication::applicationName())
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
    handlerOpen(FileIO::getFilePathOpen(this, FileIO::DateiTyp::EPL));
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
    try {
        handlerPrepareSave();
        datei->speichern();
        handlerOnSuccessfullSave();
    } catch (FilePathInvalidException& e) {
        on_actionSaveas_triggered();
    } catch (FileException& e) {
        QMessageBox::warning(this, tr("Fehler beim Speichern"), e.getError());
    }
}
void CoreMainWindow::on_actionSaveas_triggered()
{
    QString newPath = FileIO::getFilePathSave(this, tr("Einsatzplan"), FileIO::DateiTyp::EPL);
    if (newPath == "") return;

    try {
        handlerPrepareSave();
        datei->speichernUnter(newPath);
        handlerOnSuccessfullSave();
    }  catch (FileException& e) {
        QMessageBox::warning(this, tr("Fehler beim Speichern"), e.getError());
    }
}
void CoreMainWindow::autoSave()
{
    datei->autoSave();
}

void CoreMainWindow::on_actionSavePersonal_triggered()
{
    QString path = FileIO::getFilePathSave(this, tr("Einsatzplan"), FileIO::DateiTyp::EPL);
    if (path == "") return;

    try {
        handlerPrepareSave();
        datei->speichernPersonal(path);
    }  catch (FileException& e) {
        QMessageBox::warning(this, tr("Fehler beim Speichern"), e.getError());
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
    if (!datei->istGespeichert()) {
        QMessageBox::StandardButton answ = QMessageBox::question(this, tr("Ungesicherte Änderungen"),
                                                                 tr("Möchten Sie die Datei wirklich schließen?\nIhre ungesicherten Änderungen gehen dann verloren!"),
                                                                 QMessageBox::Close|QMessageBox::Cancel|QMessageBox::Save, QMessageBox::Save);
        if (answ == QMessageBox::Save) {
            on_actionSave_triggered();
            if (datei->istGespeichert()) {
                toClose = true;
            }
        } else if (answ == QMessageBox::Close) {
            toClose = true;
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
        datei->close();
        QMainWindow::closeEvent(event);
    } else {
        event->ignore();
    }
}

void CoreMainWindow::unsave()
{
    updateWindowHeaders();
}

void CoreMainWindow::handlerOnSuccessfullSave()
{
    setWindowTitle(tr("Übersicht"));
    updateWindowHeaders();
}

void CoreMainWindow::handlerPreferenes()
{
    EinstellungenDialog *dialog = new EinstellungenDialog();
    dialog->show();
}

EplFile *CoreMainWindow::open(QString path)
{
    if (path == "") return nullptr;
    EplFile* datei;
    try {
        datei = new EplFile(path);
    }  catch (FileException& e) {
        QMessageBox::warning(nullptr, tr("Fehler"), e.getError());
        return nullptr;
    }
    if (datei->istSchreibgeschuetzt()) {
        QStringList info = datei->getInfoSchreibschutz();
        QMessageBox::information(nullptr, tr("Schreibgeschützt"),
                                 tr("Die ausgewählte Datei wurde am %1 von \"%2\" zuletzt geöffnet und wird seitdem bearbeitet. Die Datei wird deshalb schreibgeschützt geöffnet!").arg(info.at(0),info.at(1)));
    }
    return datei;
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

void CoreMainWindow::updateWindowHeaders()
{
    bool mod = !datei->istGespeichert();
    setWindowModified(mod);
    QList<QMainWindow*> list = getChildWindows();
    for(QMainWindow *m: list) {
        m->setWindowModified(mod);
    }

    QString path = datei->getPfad();
    if (path != "") {
        setWindowFilePath(path);
        for(QMainWindow *mw: list) {
            mw->setWindowFilePath(path);
        }
    }
}
