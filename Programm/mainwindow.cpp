#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "calendar.h"
#include "export.h"
#include "exportgesamt.h"
#include "fileio.h"
#include "coreapplication.h"
#include "preferencesdialog.h"
#include "filesettings.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QApplication::windowIcon());
    connect(ui->calendar, SIGNAL(showFahrtag(Fahrtag*)), this, SLOT(openFahrtag(Fahrtag*)));
    connect(ui->calendar, SIGNAL(showActivity(Activity*)), this, SLOT(openActivity(Activity*)));
    connect(ui->calendar, SIGNAL(changed()), this, SLOT(unsave()));
    connect(ui->actionNeuer_Arbeitseinsatz, SIGNAL(triggered(bool)), ui->calendar, SLOT(newActivity()));
    connect(ui->actionNeuer_Fahrtag, SIGNAL(triggered(bool)), ui->calendar, SLOT(newFahrtag()));

    fenster = QMap<AActivity*, QMainWindow*>();
    ui->calendar->setPersonal(new ManagerPersonal());

    setWindowTitle(tr("Neues Dokument – Übersicht"));
    filePath = "";
    saved = true;
    setWindowModified(false);

    personalfenster = new PersonalWindow(this, ui->calendar->getPersonal());
    settings = new ManagerFileSettings();
    exportDialog = new ExportGesamt(ui->calendar, settings, this);
    connect(personalfenster, SIGNAL(changed()), this, SLOT(unsave()));
}

MainWindow::~MainWindow()
{
    this->close();
    delete ui;
}

QString MainWindow::getFarbeZug(Fahrtag::Art cat)
{
    switch (cat) {
    case Fahrtag::Museumszug:           return "#ffffff"; // Museumszug
    case Fahrtag::Sonderzug:            return "#ffcccc"; // Sonderzug -
    case Fahrtag::Gesellschaftssonderzug: return "#ffcc66"; // Gesellschaft -
    case Fahrtag::Nikolauszug:          return "#ffccff"; // Nikolausfahrt -
    case Fahrtag::ELFundMuseumszug:     return "#e7e7fd"; // Museumszug mit ELF -
    case Fahrtag::Schnupperkurs:        return "#918fe3"; // ELF-Schnupperkurs -
    case Fahrtag::Bahnhofsfest:         return "#80e3b1"; // Bahnhofsfest
    case Fahrtag::Sonstiges:            return "#ffeb90"; // Sonstiges
    default:                            return "#dddddd";
    }
}

QString MainWindow::getFarbeArbeit()
{
    return "#CCBEBE";
}

QString MainWindow::getFarbe(AActivity *a)
{
    if (Fahrtag *f = dynamic_cast<Fahrtag*>(a))
        return getFarbeZug(f->getArt());
    else
        return getFarbeArbeit();
}

void MainWindow::open(QString filepath)
{
    MainWindow *newOpen = new MainWindow();
    if (newOpen->openFile(filepath)) {
        newOpen->show();
    } else {
        newOpen->close();
    }
}

void MainWindow::openFahrtag(Fahrtag *f)
{
    if (fenster.contains(f)) {
        fenster.value(f)->show();
        fenster.value(f)->setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
        fenster.value(f)->raise();  // for MacOS
        fenster.value(f)->activateWindow(); // for Windows
    } else {
        FahrtagWindow *w = new FahrtagWindow(this, f);
        w->setWindowFilePath(filePath);
        fenster.insert(f, w);
        w->show();
    }
}

void MainWindow::openActivity(Activity *a)
{
    if (fenster.contains(a)) {
        fenster.value(a)->show();
        fenster.value(a)->setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
        fenster.value(a)->raise();  // for MacOS
        fenster.value(a)->activateWindow(); // for Windows
    } else {
        ActivityWindow *w = new ActivityWindow(this, a);
        w->setWindowFilePath(filePath);
        fenster.insert(a, w);
        w->show();
    }
}

void MainWindow::unsave()
{
    this->saved = false;
    setWindowModified(true);
}

void MainWindow::closeEvent(QCloseEvent *event)
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
        personalfenster->close();
        for(QMainWindow *m: fenster.values()) {
            m->close();
        }
    }

    if (toClose) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::on_buttonPersonal_clicked()
{
    personalfenster->show();
    personalfenster->setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    personalfenster->raise();
    personalfenster->activateWindow();
}

void MainWindow::on_buttonExport_clicked()
{
    exportDialog->hardReload();
    exportDialog->exec();
}

void MainWindow::on_actionPreferences_triggered()
{
    PreferencesDialog *dialog = new PreferencesDialog();
    dialog->setWindowFilePath("");
    dialog->show();
}

void MainWindow::on_actionAboutQt_triggered()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::on_actionAboutApp_triggered()
{
    QMessageBox::about(this, tr("Über Einsatzplaner"), "<h1>Einsatzplaner</h1>"+QCoreApplication::applicationVersion()+(CoreApplication::isDeveloperVersion()?" (dev) ":"")+ "<br/>Copyright © 2016-2018 Philipp Schepper<br/>Alle Rechte vorbehalten.");
}

void MainWindow::on_actionQuit_triggered()
{
    CoreApplication::closeAllWindows();
}

void MainWindow::on_actionNew_triggered()
{
    MainWindow *w = new MainWindow();
    w->show();
}

void MainWindow::on_actionOpen_triggered()
{
    QString file = FileIO::getFilePathOpen(this, tr("AkO-Dateien (*.ako)"));
    if (file != "") open(file);
}

void MainWindow::on_menuRecentlyused_aboutToShow()
{
    QStringList list = FileIO::getLastUsed();

    if (list.length() > 0) {
        QList<QAction*> actions = ui->menuRecentlyused->actions();
        for (int i = 2; i < actions.length(); i++) {
            ui->menuRecentlyused->removeAction(actions[i]);
        }
        ui->actionClear->setEnabled(true);
        foreach (QString entry, list) {
            QAction *a = new QAction(entry, this);
            connect(a, SIGNAL(triggered(bool)), this, SLOT(open()));
            ui->menuRecentlyused->insertAction(nullptr, a);
        }
    }
}

void MainWindow::open()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) open(action->text());
}

void MainWindow::on_actionClear_triggered()
{
    FileIO::clearLastUsed();
    QList<QAction*> actions = ui->menuRecentlyused->actions();
    for (int i = 2; i < actions.length(); i++) {
        ui->menuRecentlyused->removeAction(actions[i]);
    }
    ui->actionClear->setEnabled(false);
}

void MainWindow::on_actionSave_triggered()
{
    if (filePath == "") {
        on_actionSaveas_triggered();
        return;
    }

    QJsonObject calendarJSON = ui->calendar->toJson();

    QJsonObject viewJSON;
    viewJSON.insert("xMain", this->x());
    viewJSON.insert("yMain", this->y());
    viewJSON.insert("widthMain", this->width());
    viewJSON.insert("heightMain", this->height());
    viewJSON.insert("xPersonal", personalfenster->x());
    viewJSON.insert("yPersonal", personalfenster->y());
    viewJSON.insert("widthPersonal", personalfenster->width());
    viewJSON.insert("heightPersonal", personalfenster->height());
/*    viewJSON.insert("showPersonal", false); CURRENTLY NOT USED*/

    QJsonObject generalJSON;
    generalJSON.insert("version", CoreApplication::getAktuelleVersion()->toStringShort());

    QJsonObject object;
    object.insert("calendar", calendarJSON);
    object.insert("view", viewJSON);
    object.insert("settings", settings->toJson());
    object.insert("general", generalJSON);

    bool erfolg = FileIO::saveJsonToFile(filePath, object);
    if (erfolg) {
        saved = true;
        setWindowModified(false);
        int result = Export::autoUploadToServer(ui->calendar, settings);
        if (result == 0)
            ui->statusBar->showMessage("Datei konnte nicht hochgeladen werden!", 5000);
        else if (result > 0)
            ui->statusBar->showMessage("Datei wurde erfolgreich hochgeladen!", 5000);

    } else {
        QMessageBox::warning(this, tr("Fehler"), tr("Das Speichern unter der angegebenen Adresse ist fehlgeschlagen!"));
        filePath = "";
    }
}

bool MainWindow::openFile(QString filePath)
{
    // Daten aus Datei laden
    QJsonObject object = FileIO::getJsonFromFile(filePath);
    if (object == QJsonObject()) return false;

    // Prüfen, ob Version kompatibel ist
    QJsonObject generalJSON = object.value("general").toObject();
    CoreApplication::Version version = CoreApplication::Version::stringToVersion(generalJSON.value("version").toString());
    if (version > CoreApplication::getAktuelleVersion() || CoreApplication::Version{-1,-1,-1} == &version) {
        QMessageBox::warning(this, tr("Nicht kompatibel"),
                             tr("Die Datei kann nicht mit dieser Version geöffnet werden.\nDas Dokument benötigt mindestens Version ")+
                             version.toString()+tr(".\nDie aktuellste Version finden Sie auf der Webseite des Programms.\nBei weiteren Fragen wenden Sie sich bitte an den Support."));
        return false;
    }
    setWindowTitle(tr("Übersicht"));
    this->filePath = filePath;
    setWindowFilePath(filePath);
    personalfenster->setWindowFilePath(filePath);

    // Daten in Manager laden und darstellen lassen
    QJsonObject calendarJSON = object.value("calendar").toObject();
    ui->calendar->fromJson(calendarJSON);
    personalfenster->loadData();

    //- Hier prüfen, ob Personalfenster angezeigt wurde und wiederherstellen der Fensterpositionen
    QJsonObject viewJSON = object.value("view").toObject();
    int x = viewJSON.value("xMain").toInt();
    int y = viewJSON.value("yMain").toInt();
    int w = viewJSON.value("widthMain").toInt();
    int h = viewJSON.value("heightMain").toInt();
    this->setGeometry(x, y, w, h);
    // Personalfenster
    x = viewJSON.value("xPersonal").toInt();
    y = viewJSON.value("yPersonal").toInt();
    w = viewJSON.value("widthPersonal").toInt();
    h = viewJSON.value("heightPersonal").toInt();
    personalfenster->setGeometry(x, y, w, h);
    if (viewJSON.value("showPersonal").toBool(false)) {
        personalfenster->show();
        personalfenster->setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
        personalfenster->raise();
        personalfenster->activateWindow();
    } else {
        personalfenster->hide();
    }

    settings->fromJson(object.value("settings").toObject());
    return true;
}


void MainWindow::on_actionSaveas_triggered()
{
    filePath = FileIO::getFilePathSave(this, tr("Einsatzplan.ako"), tr("AkO-Dateien (*.ako)"));
    if (filePath != "") {
        QJsonObject o;
        bool erfolg = FileIO::saveJsonToFile(filePath, o);
        if (! erfolg) {
            QMessageBox::information(this, tr("Fehler"), tr("Das speichern unter der angegebenen Adresse ist fehlgeschlagen!"));
            filePath = "";
            return;
        }
        setWindowTitle(tr("Übersicht"));
        setWindowFilePath(filePath);
        personalfenster->setWindowFilePath(filePath);
        for(QMainWindow *mw: fenster.values()) {
            mw->setWindowFilePath(filePath);
        }
        on_actionSave_triggered();
    }
}

void MainWindow::on_actionSavePersonal_triggered()
{
    QString filePath = FileIO::getFilePathSave(this, tr("Einsatzplan.ako"), tr("AkO-Dateien (*.ako)"));
    if (filePath == "") {
        return;
    }

    QJsonObject calendarJSON = ui->calendar->personalToJson();

    QJsonObject viewJSON;
    viewJSON.insert("xMain", this->x());
    viewJSON.insert("yMain", this->y());
    viewJSON.insert("widthMain", this->width());
    viewJSON.insert("heightMain", this->height());
    viewJSON.insert("xPersonal", personalfenster->x());
    viewJSON.insert("yPersonal", personalfenster->y());
    viewJSON.insert("widthPersonal", personalfenster->width());
    viewJSON.insert("heightPersonal", personalfenster->height());
/*    viewJSON.insert("showPersonal", false); CURRENTLY NOT USED*/

    QJsonObject generalJSON;
    generalJSON.insert("version", CoreApplication::getAktuelleVersion()->toStringShort());

    QJsonObject object;
    object.insert("calendar", calendarJSON);
    object.insert("view", viewJSON);
    object.insert("general", generalJSON);

    if (! FileIO::saveJsonToFile(filePath, object)) {
        QMessageBox::warning(this, tr("Fehler"), tr("Das speichern unter der angegebenen Adresse ist fehlgeschlagen!"));
    }
}

void MainWindow::on_actionSettings_triggered()
{
    FileSettings s(this, settings);
    if (s.exec() == QDialog::Accepted) {
        s.getSettings(settings);
        emit unsave();

        // Update the settings
    }
}

bool MainWindow::on_actionClose_triggered()
{
    return close();
}
