#include "mainwindowevents.h"
#include "ui_mainwindowevents.h"
#include "exportdialog.h"
#include "eplexception.h"

#include <QMessageBox>
#include <QJsonArray>

using namespace EplException;

MainWindowEvents::MainWindowEvents(EplFile *file) : CoreMainWindow(file), ui(new Ui::MainWindowEvents)
{
    ui->setupUi(this);
    // Modell
    ui->listView->setModel(manager);
    ui->listView->setModelColumn(3);
    ui->listView->show();

    // Views
    personalfenster = new PersonalWindow(this, personal);
    exportDialog = new ExportDialog(manager, datei->getDateiEigenschaften(), this);
    recentlyUsedMenu = ui->menuRecentlyused;
    recentlyUsedClear = ui->actionClear;

    // Controller
    connect(ui->actionPreferences, &QAction::triggered, this, &MainWindowEvents::showPreferences);
    connect(ui->actionAboutQt, &QAction::triggered, this, &MainWindowEvents::showAboutQt);
    connect(ui->actionAboutApp, &QAction::triggered, this, &MainWindowEvents::showAboutApp);
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindowEvents::closeApp);

    connect(ui->actionNew, &QAction::triggered, this, &MainWindowEvents::fileNew);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindowEvents::fileOpen);

    connect(ui->menuRecentlyused, &QMenu::aboutToShow, this, &MainWindowEvents::updateRecentlyused);
    connect(ui->actionClear, &QAction::triggered, this, &MainWindowEvents::clearRecentlyUsed);

    connect(ui->actionSave, &QAction::triggered, this, &MainWindowEvents::fileSave);
    connect(ui->actionSaveas, &QAction::triggered, this, &MainWindowEvents::fileSaveAs);
    connect(ui->actionSavePersonal, &QAction::triggered, this, &MainWindowEvents::fileSavePersonal);

    connect(ui->actionSettings, &QAction::triggered, this, &MainWindowEvents::showFileSettings);
    connect(ui->actionClose, &QAction::triggered, this, &MainWindowEvents::fileClose);


    connect(ui->actionExport, &QAction::triggered, this, &MainWindowEvents::showExportDialog);
    connect(ui->actionLoeschen, &QAction::triggered, this, &MainWindowEvents::deleteSelectedInList);
    connect(ui->actionPersonal, &QAction::triggered, this, &MainWindowEvents::showPersonal);

    connect(ui->buttonPrev, &QPushButton::clicked, this, &MainWindowEvents::showPreviousMonth);
    connect(ui->buttonToday, &QPushButton::clicked, this, &MainWindowEvents::showCurrentMonth);
    connect(ui->buttonNext, &QPushButton::clicked, this, &MainWindowEvents::showNextMonth);

    connect(ui->actionNeuArbeitseinsatz, &QAction::triggered, this, [=]() { newActivity(); });
    connect(ui->actionNeuFahrtag, &QAction::triggered, this, [=]() { newFahrtag(); });
    connect(ui->dateSelector, &QDateEdit::dateChanged, this, &MainWindowEvents::showDate);
    connect(ui->listView, &QListView::doubleClicked, this, [=](QModelIndex index) { openAktivitaet(manager->getActivities().at(index.row())); });

    // Setup fuer die Darstellung des Kalenders
    tage = QList<CalendarDay*>();
    tage << ui->day1_1 << ui->day2_1 << ui->day3_1 << ui->day4_1 << ui->day5_1 << ui->day6_1 << ui->day7_1;
    tage << ui->day1_2 << ui->day2_2 << ui->day3_2 << ui->day4_2 << ui->day5_2 << ui->day6_2 << ui->day7_2;
    tage << ui->day1_3 << ui->day2_3 << ui->day3_3 << ui->day4_3 << ui->day5_3 << ui->day6_3 << ui->day7_3;
    tage << ui->day1_4 << ui->day2_4 << ui->day3_4 << ui->day4_4 << ui->day5_4 << ui->day6_4 << ui->day7_4;
    tage << ui->day1_5 << ui->day2_5 << ui->day3_5 << ui->day4_5 << ui->day5_5 << ui->day6_5 << ui->day7_5;
    tage << ui->day1_6 << ui->day2_6 << ui->day3_6 << ui->day4_6 << ui->day5_6 << ui->day6_6 << ui->day7_6;
    for(CalendarDay *c: std::as_const(tage)) {
        c->setModel(manager);
        connect(c, &CalendarDay::clickedItem, this, [=](QModelIndex index) {
                MainWindowEvents::openAktivitaet(manager->getData(index));
        });
        connect(c, &CalendarDay::addActivity, this, &MainWindowEvents::newActivity);
    }


    if (datei->istSchreibgeschuetzt()) {
        setWindowTitle(tr("Übersicht - Schreibgeschützt"));
    }

    QDate currentDate = datei->getAnzeigeDatum();
    // an das gespeicherte Datum gehen
    ui->dateSelector->setDate(currentDate);
    ui->dateSelector->repaint();
    showDate(currentDate);

    personalfenster->refresh();

    //- Hier prüfen, ob Personalfenster angezeigt wurde und wiederherstellen der Fensterpositionen
    EplFile::FensterPosition kalender = datei->getPositionKalender();
    if (kalender.width + kalender.height > 0)
        this->setGeometry(kalender.x, kalender.y, kalender.width, kalender.height);
    // Personalfenster
    EplFile::FensterPosition personal = datei->getPositionPersonal();
    if (personal.width + personal.height > 0)
        personalfenster->setGeometry(personal.x, personal.y, personal.width, personal.height);
    personalfenster->hide();
}
MainWindowEvents::~MainWindowEvents()
{
    delete ui;
}

bool MainWindowEvents::open(QString path)
{
    EplFile* datei = getDateiVonPfad(path);
    if (datei == nullptr) return false;

    CoreMainWindow *mw = new MainWindowEvents(datei);
    mw->show();
    return true;
}


CoreMainWindow* MainWindowEvents::handlerNew()
{
    return new MainWindowEvents();
}
void MainWindowEvents::handlerOpen(QString path)
{
    open(path);
}

void MainWindowEvents::onDateiWirdGespeichertWerden()
{
    CoreMainWindow::onDateiWirdGespeichertWerden();

    datei->setPositionKalender(EplFile::FensterPosition {x(), y(), width(), height() });
    datei->setPositionPersonal(
                EplFile::FensterPosition {personalfenster->x(),
                                          personalfenster->y(),
                                          personalfenster->width(),
                                          personalfenster->height() });
    datei->setAnzeigeDatum(ui->dateSelector->date());
}
void MainWindowEvents::onDateiWurdeErfolgreichGespeichert()
{
    CoreMainWindow::onDateiWurdeErfolgreichGespeichert();
    try {
        datei->autoUpload();
        ui->statusBar->showMessage(tr("Datei wurde erfolgreich hochgeladen!"), 5000);
    } catch (KeinAutoUploadException &e) {

    } catch (NetworkingException &e) {
        QMessageBox::warning(this, tr("Fehler beim Hochladen"), e.getError());
    }
}

void MainWindowEvents::onAktivitaetWirdEntferntWerden(AActivity *a)
{
    CoreMainWindow::onAktivitaetWirdEntferntWerden(a);
}
void MainWindowEvents::onAktivitaetWurdeBearbeitet(AActivity *a, QDate altesDatum)
{
    CoreMainWindow::onAktivitaetWurdeBearbeitet(a, altesDatum);
}


void MainWindowEvents::showExportDialog()
{
    exportDialog->exec();
}

void MainWindowEvents::deleteSelectedInList()
{
    QModelIndexList indexList = ui->listView->selectionModel()->selection().indexes();
    for(QModelIndex index: std::as_const(indexList)) {
        loeschenAktivitaet(manager->getData(index));
    }
}

void MainWindowEvents::showPersonal()
{
    personalfenster->show();
    personalfenster->setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    personalfenster->raise();
    personalfenster->activateWindow();
}


void MainWindowEvents::showPreviousMonth()
{
    ui->dateSelector->setDate(ui->dateSelector->date().addDays(-7));
    ui->dateSelector->repaint();
}
void MainWindowEvents::showNextMonth()
{
    ui->dateSelector->setDate(ui->dateSelector->date().addDays(7));
    ui->dateSelector->repaint();
}
void MainWindowEvents::showCurrentMonth()
{
    QDate cur = QDate::currentDate();
    ui->dateSelector->setDate(cur.addDays(-cur.day()+1));
    ui->dateSelector->repaint();
}
void MainWindowEvents::showDate(QDate date)
{
//    date = date.addDays(-date.day()+1); // Datum auf Monatsanfang setzen
    QDate akt = date.addDays(-date.dayOfWeek()+1);

    // Eintragen der Wochennummern
    ui->number1_1->setText(QString::number(akt.addDays(0).weekNumber()));
    ui->number1_2->setText(QString::number(akt.addDays(7).weekNumber()));
    ui->number1_3->setText(QString::number(akt.addDays(14).weekNumber()));
    ui->number1_4->setText(QString::number(akt.addDays(21).weekNumber()));
    ui->number1_5->setText(QString::number(akt.addDays(28).weekNumber()));
    ui->number1_6->setText(QString::number(akt.addDays(35).weekNumber()));

    int displayMonth = akt.month();
    if (akt.daysInMonth() - akt.day() < 20) displayMonth = akt.addMonths(1).month();
    // Einstellen der einzelnen Tage
    for (int i = 0; i < tage.length(); i++) {
        tage.at(i)->show(akt, akt.month() != displayMonth);
        akt = akt.addDays(1);
    }
}


void MainWindowEvents::newFahrtag(QDate d)
{
    Fahrtag *f = manager->newFahrtag(d);
    newAActivityHandler(f);
    openAktivitaet(f);
}
void MainWindowEvents::newActivity(QDate d)
{
    AActivity *a = manager->newActivity(d);
    newAActivityHandler(a);
    openAktivitaet(a);
}
void MainWindowEvents::newAActivityHandler(AActivity *a)
{
    onAktivitaetWurdeBearbeitet(a);
}
