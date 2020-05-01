#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "export.h"
#include "exportgesamt.h"
#include "fileio.h"
#include "coreapplication.h"
#include "preferencesdialog.h"
#include "filesettings.h"
#include "fahrtagwindow.h"
#include "activitywindow.h"

#include <QMessageBox>
#include <QJsonArray>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Modell
    manager = new Manager();
    filePath = "";
    saved = true;
    // Views
    personalfenster = new PersonalWindow(this, manager->getPersonal());
    settings = new ManagerFileSettings();
    exportDialog = new ExportGesamt(manager, settings, this);
    // Controller
    listitem = QMap<AActivity*, QListWidgetItem*>();
    itemToList = QMap<QListWidgetItem*, AActivity*>();
    fenster = QMap<AActivity*, QMainWindow*>();

    setWindowTitle(tr("Neues Dokument – Übersicht"));
    setWindowModified(false);
    setWindowIcon(QApplication::windowIcon());

    connect(manager->getPersonal(), SIGNAL(changed()), this, SLOT(unsave()));
    connect(ui->buttonAddActivity, SIGNAL(clicked(bool)), this, SLOT(newActivity()));
    connect(ui->buttonAddFahrtag, SIGNAL(clicked(bool)), this, SLOT(newFahrtag()));
    connect(ui->dateSelector, SIGNAL(dateChanged(QDate)), this, SLOT(showDate(QDate)));
    connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(onItemInListClicked(QListWidgetItem*)));

    // Setup fuer die Darstellung des Kalenders
    tage = QList<CalendarDay*>();
    tage.append(ui->day1_1);    tage.append(ui->day2_1);    tage.append(ui->day3_1);    tage.append(ui->day4_1);
    tage.append(ui->day5_1);    tage.append(ui->day6_1);    tage.append(ui->day7_1);
    tage.append(ui->day1_2);    tage.append(ui->day2_2);    tage.append(ui->day3_2);    tage.append(ui->day4_2);
    tage.append(ui->day5_2);    tage.append(ui->day6_2);    tage.append(ui->day7_2);
    tage.append(ui->day1_3);    tage.append(ui->day2_3);    tage.append(ui->day3_3);    tage.append(ui->day4_3);
    tage.append(ui->day5_3);    tage.append(ui->day6_3);    tage.append(ui->day7_3);
    tage.append(ui->day1_4);    tage.append(ui->day2_4);    tage.append(ui->day3_4);    tage.append(ui->day4_4);
    tage.append(ui->day5_4);    tage.append(ui->day6_4);    tage.append(ui->day7_4);
    tage.append(ui->day1_5);    tage.append(ui->day2_5);    tage.append(ui->day3_5);    tage.append(ui->day4_5);
    tage.append(ui->day5_5);    tage.append(ui->day6_5);    tage.append(ui->day7_5);
    tage.append(ui->day1_6);    tage.append(ui->day2_6);    tage.append(ui->day3_6);    tage.append(ui->day4_6);
    tage.append(ui->day5_6);    tage.append(ui->day6_6);    tage.append(ui->day7_6);
    for(CalendarDay *c: tage) {
        connect(c, SIGNAL(clickedItem(AActivity*)), this, SLOT(itemInCalendarDayClicked(AActivity*)));
        connect(c, SIGNAL(addActivity(QDate)), this, SLOT(newActivity(QDate)));
    }
    on_buttonToday_clicked();
}

MainWindow::MainWindow(QJsonObject json, QString path) : MainWindow()
{
    setWindowTitle(tr("Übersicht"));
    filePath = path;
    setWindowFilePath(filePath);
    personalfenster->setWindowFilePath(filePath);

    // Daten in Manager laden und darstellen lassen
    QJsonArray activities;
    QJsonObject personal;
    QDate currentDate;
    if (json.contains("personal") && json.contains("activities")) {
        // Ab Version 1.6
        activities = json.value("activities").toArray();
        personal = json.value("personal").toObject();
        currentDate = QDate::fromString(json.value("view").toObject().value("currentDate").toString(), "yyyy-MM-dd");
    } else {
        // Fallback fuer Version <1.6
        QJsonObject calendarJSON = json.value("calendar").toObject();
        activities = calendarJSON.value("activites").toArray();
        personal = calendarJSON.value("personal").toObject();
        // Daten in den Manager laden und die Logik herstellen
        currentDate = QDate::fromString(calendarJSON.value("currentDate").toString(), "yyyy-MM-dd");
    }
    manager->getPersonal()->fromJson(personal);
    manager->fromJson(activities);
    // Alle aktivitäten in die seitenleiste eintragen
    for(AActivity *a: manager->getActivities()) {
        QListWidgetItem *i = new QListWidgetItem("");
        connect(a, SIGNAL(changed(AActivity*, QDate)), this, SLOT(activityChanged(AActivity*, QDate)));
        connect(a, SIGNAL(del(AActivity*)), this, SLOT(removeActivity(AActivity*)));
        setListItem(i, a);
        ui->listWidget->insertItem(ui->listWidget->count(), i);
        listitem.insert(a, i);
        itemToList.insert(i, a);
    }
    // an das gespeicherte Datum gehen
    ui->dateSelector->setDate(currentDate);
    ui->dateSelector->repaint();
    showDate(currentDate);

    personalfenster->refresh();

    //- Hier prüfen, ob Personalfenster angezeigt wurde und wiederherstellen der Fensterpositionen
    QJsonObject viewJSON = json.value("view").toObject();
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
    personalfenster->hide();

    settings->fromJson(json.value("settings").toObject());
}

MainWindow::~MainWindow()
{
    this->close();
    delete ui;
}

void MainWindow::newFahrtag(QDate d)
{
    Fahrtag *f = manager->newFahrtag(d);
    newAActivityHandler(f);
    openFahrtag(f);
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

void MainWindow::newActivity(QDate d)
{
    Activity *a = manager->newActivity(d);
    newAActivityHandler(a);
    openActivity(a);
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

void MainWindow::newAActivityHandler(AActivity *a)
{
    unsave();

    connect(a, SIGNAL(changed(AActivity*, QDate)), this, SLOT(activityChanged(AActivity*, QDate)));
    connect(a, SIGNAL(del(AActivity*)), this, SLOT(removeActivity(AActivity*)));

    // Einfügen in die Seitenliste
    ui->listWidget->insertItem(ui->listWidget->count(), a->getListString());
    QListWidgetItem *i = ui->listWidget->item(ui->listWidget->count()-1);
    listitem.insert(a, i);
    itemToList.insert(i, a);

    activityChanged(a);
}

void MainWindow::activityChanged(AActivity *a, QDate oldDate)
{
    manager->activityChanged(a);
    int oldPos = -1;
    if (oldDate.isValid())
        oldPos = getPosInCalendar(oldDate);

    if (oldPos < 42 && oldPos >= 0) {
        tage.at(oldPos)->remove(a);
    }

    // Richtiges Anzeigen im Kalender
    int pos = getPosInCalendar(a->getDatum());
    if (pos < 42 && pos >= 0) {
        tage.at(pos)->insert(a);
    }

    // Richtiges Anzeigen in der Übersichts liste
    setListItem(listitem.value(a), a);

    // Prüfen, ob das element weitgenug vorne (oben) ist
    int i = ui->listWidget->row(listitem.value(a));
    AActivity *ref;
    while (i > 0) {
        ref = itemToList.value(ui->listWidget->item(i-1));
        if (*a < *ref) {
            ui->listWidget->insertItem(i-1, ui->listWidget->takeItem(i));
            i--;
        } else {
            break;
        }
    }

    // Prüfen, ob das element weit genug hinten (unten) ist
    while (i < ui->listWidget->count()-1) {
        ref = itemToList.value(ui->listWidget->item(i+1));
        if (*ref < *a) {
            ui->listWidget->insertItem(i, ui->listWidget->takeItem(i+1));
            i++;
        } else {
            break;
        }
    }
    unsave();
}

void MainWindow::on_buttonDelete_clicked()
{
    QList<QListWidgetItem*> selected = ui->listWidget->selectedItems();
    if (! selected.isEmpty()) {
        if (QMessageBox::question(this, tr("Wirklich löschen?"), tr("Möchten Sie die ausgewählten Aktivitäten wirklich unwiderruflich löschen?")) == QMessageBox::Yes) {
            for(QListWidgetItem *item: selected) {
                removeActivity(itemToList.value(item));
            }
        }
    }
}
bool MainWindow::removeActivity(AActivity *a)
{
    ui->listWidget->takeItem(ui->listWidget->row(listitem.value(a)));
    itemToList.remove(listitem.value(a));
    listitem.remove(a);
    tage.at(getPosInCalendar(a->getDatum()))->remove(a);
    bool ret = manager->removeActivity(a);
    unsave();
    if (fenster.contains(a))
        fenster.remove(a);
    return ret;
}

void MainWindow::showDate(QDate date)
{
    date = date.addDays(-date.day()+1); // Datum auf Monatsanfang setzen
    QDate akt = date.addDays(-date.dayOfWeek()+1);

    // Eintragen der Wochennummern
    ui->number1_1->setText(QString::number(akt.addDays(0).weekNumber()));
    ui->number1_2->setText(QString::number(akt.addDays(7).weekNumber()));
    ui->number1_3->setText(QString::number(akt.addDays(14).weekNumber()));
    ui->number1_4->setText(QString::number(akt.addDays(21).weekNumber()));
    ui->number1_5->setText(QString::number(akt.addDays(28).weekNumber()));
    ui->number1_6->setText(QString::number(akt.addDays(35).weekNumber()));

    // Einstellen der einzelnen Tage
    for (int i = 0; i < tage.length(); i++) {
        tage.at(i)->show(akt, akt.month() != date.month());
        akt = akt.addDays(1);
    }

    foreach (AActivity *a, manager->getActivities()) {
        int pos = getPosInCalendar(a->getDatum());
        if (pos != -1) {
            tage.at(pos)->insert(a);
        }
    }
}

void MainWindow::on_buttonPrev_clicked()
{
    ui->dateSelector->setDate(ui->dateSelector->date().addMonths(-1));
    ui->dateSelector->repaint();
}
void MainWindow::on_buttonNext_clicked()
{
    ui->dateSelector->setDate(ui->dateSelector->date().addMonths(1));
    ui->dateSelector->repaint();
}
void MainWindow::on_buttonToday_clicked()
{
    ui->dateSelector->setDate(QDate::currentDate());
    ui->dateSelector->repaint();
}

void MainWindow::itemInCalendarDayClicked(AActivity *a)
{
    if (Fahrtag *f = dynamic_cast<Fahrtag*>(a)) {
        openFahrtag(f);
    } else {
        Activity *c = dynamic_cast<Activity*>(a);
        openActivity(c);
    }
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
    QMessageBox::about(this, tr("Über Einsatzplaner"),tr("<h1>Einsatzplaner</h1>%1 %2<br/>2016-2020 Philipp Schepper").arg(QCoreApplication::applicationVersion()).arg(CoreApplication::DEVELOPER_MODE ? "(dev)" : ""));
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
    open(FileIO::getFilePathOpen(this, tr("AkO-Dateien (*.ako)")));
}
void MainWindow::open(QString path)
{
    if (path == "") return;
    // Daten aus Datei laden
    QJsonObject object = FileIO::getJsonFromFile(path);
    if (object.isEmpty()) return;

    // Prüfen, ob Version kompatibel ist
    QJsonObject generalJSON = object.value("general").toObject();

    Version version = Version::stringToVersion(generalJSON.value("version").toString());
    if (version > (CoreApplication::VERSION) || Version{-1,-1,-1} == version) {
        QMessageBox::warning(nullptr, tr("Nicht kompatibel"),
                             tr("Die Datei kann nicht mit dieser Version geöffnet werden.\n"
                                "Das Dokument benötigt mindestens Version %1.\n"
                                "Die aktuellste Version finden Sie auf der Webseite des Programms.\n"
                                "Bei weiteren Fragen wenden Sie sich bitte an den Support.").arg(version.toString()));
        return;
    }
    MainWindow *mw = new MainWindow(object, path);
    mw->show();
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
            connect(a, &QAction::triggered, this, [=]() { open(entry); });
            ui->menuRecentlyused->insertAction(nullptr, a);
        }
    }
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

    bool erfolg = saveToPath(filePath);
    if (erfolg) {
        saved = true;
        setWindowModified(false);
        if (filePath != "") {
            QFile file (filePath+".autosave.ako");
            file.remove();
        }

        int result = Export::autoUploadToServer(settings, manager);
        if (result == 0)
            ui->statusBar->showMessage(tr("Datei konnte nicht hochgeladen werden!"), 5000);
        else if (result > 0)
            ui->statusBar->showMessage(tr("Datei wurde erfolgreich hochgeladen!"), 5000);

    } else {
        QMessageBox::warning(this, tr("Fehler"), tr("Das Speichern unter der angegebenen Adresse ist fehlgeschlagen!"));
        filePath = "";
    }
}
void MainWindow::on_actionSaveas_triggered()
{
    QString newPath = FileIO::getFilePathSave(this, tr("Einsatzplan.ako"), tr("AkO-Dateien (*.ako)"));
    if (newPath != "") {
        filePath = newPath;
        QJsonObject o;
        bool erfolg = FileIO::saveJsonToFile(filePath, o);
        if (! erfolg) {
            QMessageBox::information(this, tr("Fehler"), tr("Das Speichern unter der angegebenen Adresse ist fehlgeschlagen!"));
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
void MainWindow::autoSave()
{
    if (filePath == "") return;
    if (saved) return;
    saveToPath(filePath+".autosave.ako", false);
}
bool MainWindow::saveToPath(QString path, bool showInMenu)
{
    QJsonArray activitiesJSON = manager->toJson();
    QJsonObject personalJSON = manager->getPersonal()->toJson();

    QJsonObject viewJSON;
    viewJSON.insert("xMain", this->x());
    viewJSON.insert("yMain", this->y());
    viewJSON.insert("widthMain", this->width());
    viewJSON.insert("heightMain", this->height());
    viewJSON.insert("xPersonal", personalfenster->x());
    viewJSON.insert("yPersonal", personalfenster->y());
    viewJSON.insert("widthPersonal", personalfenster->width());
    viewJSON.insert("heightPersonal", personalfenster->height());
    viewJSON.insert("currentDate", ui->dateSelector->date().toString("yyyy-MM-dd"));

    QJsonObject generalJSON;
    generalJSON.insert("version", CoreApplication::VERSION.toStringShort());

    QJsonObject json;
    json.insert("activities", activitiesJSON);
    json.insert("personal", personalJSON);
    json.insert("view", viewJSON);
    json.insert("settings", settings->toJson());
    json.insert("general", generalJSON);

    return FileIO::saveJsonToFile(path, json, showInMenu);
}

void MainWindow::on_actionSavePersonal_triggered()
{
    QString path = FileIO::getFilePathSave(this, tr("Einsatzplan.ako"), tr("AkO-Dateien (*.ako)"));
    if (path == "") return;

    QJsonObject personalJSON = manager->getPersonal()->personalToJson();

    QJsonObject viewJSON;
    viewJSON.insert("xMain", this->x());
    viewJSON.insert("yMain", this->y());
    viewJSON.insert("widthMain", this->width());
    viewJSON.insert("heightMain", this->height());
    viewJSON.insert("xPersonal", personalfenster->x());
    viewJSON.insert("yPersonal", personalfenster->y());
    viewJSON.insert("widthPersonal", personalfenster->width());
    viewJSON.insert("heightPersonal", personalfenster->height());
    viewJSON.insert("currentDate", ui->dateSelector->date().toString("yyyy-MM-dd"));

    QJsonObject generalJSON;
    generalJSON.insert("version", CoreApplication::VERSION.toStringShort());

    QJsonObject object;
    object.insert("personal", personalJSON);
    object.insert("view", viewJSON);
    object.insert("settings", settings->toJson());
    object.insert("general", generalJSON);

    if (! FileIO::saveJsonToFile(path, object)) {
        QMessageBox::warning(this, tr("Fehler"), tr("Das Speichern unter der angegebenen Adresse ist fehlgeschlagen!"));
    }
}

void MainWindow::on_actionSettings_triggered()
{
    FileSettings s(this, settings);
    if (s.exec() == QDialog::Accepted) {
        s.getSettings(settings);
        emit unsave();
    }
}

bool MainWindow::on_actionClose_triggered()
{
    return close();
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
        if (filePath != "") {
            QFile file (filePath+".autosave.ako");
            file.remove();
        }
    }

    if (toClose) {
        event->accept();
    } else {
        event->ignore();
    }
}


void MainWindow::onItemInListClicked(QListWidgetItem *item)
{
    AActivity *a = itemToList.value(item);
    if (Fahrtag *f = dynamic_cast<Fahrtag*>(a)) {
        openFahrtag(f);
    } else {
        Activity *c = dynamic_cast<Activity*>(a);
        openActivity(c);
    }
}
void MainWindow::setListItem(QListWidgetItem *i, AActivity *a)
{
    if (i == nullptr) return;
    i->setText(a->getListString());
    i->setToolTip(a->getAnlass());
    i->setWhatsThis(a->getAnlass());
    i->setBackground(QBrush(QColor(getFarbe(a))));
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

void MainWindow::unsave()
{
    this->saved = false;
    setWindowModified(true);
}

int MainWindow::getPosInCalendar(QDate date)
{
    QDate start = ui->dateSelector->date();
    start = start.addDays(-start.day()+1); // Datum auf Monatsanfang setzen
    start = start.addDays(-start.dayOfWeek()+1);

    long long diff = start.daysTo(date);
    if (diff < 0 || diff >= tage.length()) {
        return -1;
    }
    return static_cast<int>(diff);
}
