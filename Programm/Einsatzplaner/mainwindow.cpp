#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "export.h"
#include "exportdialog.h"
#include "fileio.h"
#include "coreapplication.h"
#include "einstellungendialog.h"
#include "filesettingsdialog.h"
#include "fahrtagwindow.h"
#include "activitywindow.h"

#include <QMessageBox>
#include <QJsonArray>

MainWindow::MainWindow(QWidget *parent) : CoreMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Modell
    manager = new Manager();

    // Views
    personalfenster = new PersonalWindow(this, manager->getPersonal());
    settings = new FileSettings();
    exportDialog = new ExportDialog(manager, settings, this);
    recentlyUsedMenu = ui->menuRecentlyused;
    recentlyUsedClear = ui->actionClear;

    // Controller
    listitem = QMap<AActivity*, QListWidgetItem*>();
    itemToList = QMap<QListWidgetItem*, AActivity*>();
    fenster = QMap<AActivity*, QMainWindow*>();

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
        connect(c, SIGNAL(clickedItem(AActivity*)), this, SLOT(openAActivity(AActivity*)));
        connect(c, SIGNAL(addActivity(QDate)), this, SLOT(newActivity(QDate)));
    }
    on_buttonToday_clicked();
}

bool MainWindow::handlerLadeDatei(QJsonObject json)
{
    if (istSchreibgeschuetzt) {
        setWindowTitle(tr("Übersicht - Schreibgeschützt"));
    }

    // Daten in Manager laden und darstellen lassen
    QJsonArray activities;
    QJsonObject personal;
    QDate currentDate;
    Version version = Version::stringToVersion(json.value("general").toObject().value("version").toString());
    if (version == Version {-1, -1, -1}) {
        QMessageBox::warning(this, tr("Datei nicht kompatibel"), tr("Die Datei kann nicht geladen werden. Wenden Sie sich an den Entwickler für mehr Informationen."));
        return false;
    } else if (version < Version {1, 6, -1}) {
        // Fallback fuer Version <1.6
        QJsonObject calendarJSON = json.value("calendar").toObject();
        activities = calendarJSON.value("activites").toArray();
        personal = calendarJSON.value("personal").toObject();
        // Daten in den Manager laden und die Logik herstellen
        currentDate = QDate::fromString(calendarJSON.value("currentDate").toString(), "yyyy-MM-dd");
    } else if (version <= CoreApplication::VERSION) {
        // Ab Version 1.6
        activities = json.value("activities").toArray();
        personal = json.value("personal").toObject();
        currentDate = QDate::fromString(json.value("view").toObject().value("currentDate").toString(), "yyyy-MM-dd");
    } else {
        // Version inkompatibel, zu neu
        QMessageBox::warning(this, tr("Datei zu neu"), tr("Die Datei kann nicht geladen werden, da sie mindestens Version %1 benötigt.").arg(version.toString()));
        return false;
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
    return true;
}

MainWindow::~MainWindow()
{
    close();
    delete ui;
}

void MainWindow::newFahrtag(QDate d)
{
    Fahrtag *f = manager->newFahrtag(d);
    newAActivityHandler(f);
    openAActivity(f);
}
void MainWindow::newActivity(QDate d)
{
    Activity *a = manager->newActivity(d);
    newAActivityHandler(a);
    openAActivity(a);
}
void MainWindow::openAActivity(AActivity *a)
{
    if (a == nullptr) return;

    if (fenster.contains(a)) {
        fenster.value(a)->show();
        fenster.value(a)->setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
        fenster.value(a)->raise();  // for MacOS
        fenster.value(a)->activateWindow(); // for Windows
    } else {
        QMainWindow *w;
        if (a->getArt() == Art::Arbeitseinsatz) {
            Activity* aa = dynamic_cast<Activity*>(a);
            w = new ActivityWindow(this, aa);
        } else {
            Fahrtag* f = dynamic_cast<Fahrtag*>(a);
            w = new FahrtagWindow(this, f);
        }
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
    manager->sort();
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
    int pos = getPosInCalendar(a->getDatum());
    if (pos >= 0)
        tage.at(pos)->remove(a);
    bool ret = manager->removeActivity(a);
    unsave();
    if (fenster.contains(a)) {
        fenster.remove(a);
    }
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

    for (AActivity *a: manager->getActivities()) {
        int pos = getPosInCalendar(a->getDatum());
        if (pos != -1) {
            tage.at(pos)->insert(a);
        }
    }
}

bool MainWindow::open(QString path)
{
    if (path == "") return false;

    // Daten aus Datei laden
    QJsonObject object = FileIO::getJsonFromFile(path);
    if (object.isEmpty()) return false;

    // Prüfen, ob Version kompatibel ist
    QJsonObject generalJSON = object.value("general").toObject();
    Version version = Version::stringToVersion(generalJSON.value("version").toString());
    if (! pruefeVersionMitWarnung(version)) return false;

    // Schreibschutz pruefen
    bool schreibschutz = !setzeSchreibschutzOderWarnung(path);

    MainWindow *mw = new MainWindow();
    mw->ladeDatei(path, object, schreibschutz);
    mw->show();
    return true;
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

void MainWindow::onItemInListClicked(QListWidgetItem *item)
{
    openAActivity(itemToList.value(item));
}
void MainWindow::setListItem(QListWidgetItem *i, AActivity *a)
{
    if (i == nullptr) return;
    i->setText(a->getListString());
    i->setToolTip(a->getAnlass().replace("<br/>","\n"));
    i->setWhatsThis(a->getAnlass().replace("<br/>","\n"));
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

CoreMainWindow* MainWindow::handlerNew()
{
    return new MainWindow();
}

QJsonObject MainWindow::handlerSave()
{
    QJsonObject json = handlerSavePersonal();
    json.insert("activities", manager->toJson());
    return json;
}

void MainWindow::handlerSaveAdditional()
{
    if (settings->getAutom()) {
        int result = Export::autoUploadToServer(manager->filter(settings->getAuswahl()), settings->getServer());
        if (result == 0)
            ui->statusBar->showMessage(tr("Datei konnte nicht hochgeladen werden!"), 5000);
        else if (result > 0)
            ui->statusBar->showMessage(tr("Datei wurde erfolgreich hochgeladen!"), 5000);
    }
    return;
}

void MainWindow::handlerOpen(QString path)
{
    MainWindow::open(path);
}

void MainWindow::handlerPreferenes()
{
    EinstellungenDialog *dialog = new EinstellungenDialog();
    dialog->show();
}

void MainWindow::handlerSettings()
{
    FileSettingsDialog s(this, settings);
    if (s.exec() == QDialog::Accepted) {
        s.getSettings(settings);
        emit unsave();
    }
}

QJsonObject MainWindow::handlerSavePersonal()
{
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

    return object;
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
