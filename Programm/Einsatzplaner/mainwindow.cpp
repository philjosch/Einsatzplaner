#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "exportdialog.h"
#include "eplexception.h"

#include <QMessageBox>
#include <QJsonArray>

using namespace EplException;

MainWindow::MainWindow(EplFile *file) : CoreMainWindow(file), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Modell

    // Views
    personalfenster = new PersonalWindow(this, personal);
    exportDialog = new ExportDialog(manager, datei->getDateiEigenschaften(), this);
    recentlyUsedMenu = ui->menuRecentlyused;
    recentlyUsedClear = ui->actionClear;

    // Controller
    listitem = QMap<AActivity*, QListWidgetItem*>();
    itemToList = QMap<QListWidgetItem*, AActivity*>();

    connect(ui->actionPreferences, &QAction::triggered, this, &MainWindow::showPreferences);
    connect(ui->actionAboutQt, &QAction::triggered, this, &MainWindow::showAboutQt);
    connect(ui->actionAboutApp, &QAction::triggered, this, &MainWindow::showAboutApp);
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::closeApp);

    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::fileNew);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::fileOpen);

    connect(ui->menuRecentlyused, &QMenu::aboutToShow, this, &MainWindow::updateRecentlyused);
    connect(ui->actionClear, &QAction::triggered, this, &MainWindow::clearRecentlyUsed);

    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::fileSave);
    connect(ui->actionSaveas, &QAction::triggered, this, &MainWindow::fileSaveAs);
    connect(ui->actionSavePersonal, &QAction::triggered, this, &MainWindow::fileSavePersonal);

    connect(ui->actionSettings, &QAction::triggered, this, &MainWindow::showFileSettings);
    connect(ui->actionClose, &QAction::triggered, this, &MainWindow::fileClose);


    connect(ui->actionExport, &QAction::triggered, this, &MainWindow::showExportDialog);
    connect(ui->actionLoeschen, &QAction::triggered, this, &MainWindow::deleteSelectedInList);
    connect(ui->actionPersonal, &QAction::triggered, this, &MainWindow::showPersonal);

    connect(ui->buttonPrev, &QPushButton::clicked, this, &MainWindow::showPreviousMonth);
    connect(ui->buttonToday, &QPushButton::clicked, this, &MainWindow::showCurrentMonth);
    connect(ui->buttonNext, &QPushButton::clicked, this, &MainWindow::showNextMonth);

    connect(ui->actionNeuArbeitseinsatz, SIGNAL(triggered(bool)), this, SLOT(newActivity()));
    connect(ui->actionNeuFahrtag, SIGNAL(triggered(bool)), this, SLOT(newFahrtag()));
    connect(ui->dateSelector, &QDateEdit::dateChanged, this, &MainWindow::showDate);
    connect(ui->listWidget, &QListWidget::itemDoubleClicked, this, [=](QListWidgetItem *item) { openAktivitaet(itemToList.value(item)); });

    // Setup fuer die Darstellung des Kalenders
    tage = QList<CalendarDay*>();
    tage << ui->day1_1 << ui->day2_1 << ui->day3_1 << ui->day4_1 << ui->day5_1 << ui->day6_1 << ui->day7_1;
    tage << ui->day1_2 << ui->day2_2 << ui->day3_2 << ui->day4_2 << ui->day5_2 << ui->day6_2 << ui->day7_2;
    tage << ui->day1_3 << ui->day2_3 << ui->day3_3 << ui->day4_3 << ui->day5_3 << ui->day6_3 << ui->day7_3;
    tage << ui->day1_4 << ui->day2_4 << ui->day3_4 << ui->day4_4 << ui->day5_4 << ui->day6_4 << ui->day7_4;
    tage << ui->day1_5 << ui->day2_5 << ui->day3_5 << ui->day4_5 << ui->day5_5 << ui->day6_5 << ui->day7_5;
    tage << ui->day1_6 << ui->day2_6 << ui->day3_6 << ui->day4_6 << ui->day5_6 << ui->day6_6 << ui->day7_6;
    for(CalendarDay *c: std::as_const(tage)) {
        connect(c, &CalendarDay::clickedItem, this, &MainWindow::openAktivitaet);
        connect(c, &CalendarDay::addActivity, this, &MainWindow::newActivity);
    }

    if (datei->istSchreibgeschuetzt()) {
        setWindowTitle(tr("Übersicht - Schreibgeschützt"));
    }

    QDate currentDate = datei->getAnzeigeDatum();
    // Alle aktivitäten in die seitenleiste eintragen
    for(AActivity *a: manager->getActivities()) {
        QListWidgetItem *i = new QListWidgetItem("");
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
    EplFile::FensterPosition kalender = datei->getPositionKalender();
    if (kalender.width + kalender.height > 0)
        this->setGeometry(kalender.x, kalender.y, kalender.width, kalender.height);
    // Personalfenster
    EplFile::FensterPosition personal = datei->getPositionPersonal();
    if (personal.width + personal.height > 0)
        personalfenster->setGeometry(personal.x, personal.y, personal.width, personal.height);
    personalfenster->hide();
}
MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::open(QString path)
{
    EplFile* datei = getDateiVonPfad(path);
    if (datei == nullptr) return false;

    CoreMainWindow *mw = new MainWindow(datei);
    mw->show();
    return true;
}


CoreMainWindow* MainWindow::handlerNew()
{
    return new MainWindow();
}
void MainWindow::handlerOpen(QString path)
{
    open(path);
}

void MainWindow::onDateiWirdGespeichertWerden()
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
void MainWindow::onDateiWurdeErfolgreichGespeichert()
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

void MainWindow::onAktivitaetWirdEntferntWerden(AActivity *a)
{
    CoreMainWindow::onAktivitaetWirdEntferntWerden(a);

    ui->listWidget->takeItem(ui->listWidget->row(listitem.value(a)));
    itemToList.remove(listitem.value(a));
    listitem.remove(a);
    int pos = getPosInCalendar(a->getDatum());
    if (pos >= 0)
        tage.at(pos)->remove(a);

}
void MainWindow::onAktivitaetWurdeBearbeitet(AActivity *a, QDate altesDatum)
{
    CoreMainWindow::onAktivitaetWurdeBearbeitet(a, altesDatum);

    int oldPos = -1;
    if (altesDatum.isValid())
        oldPos = getPosInCalendar(altesDatum);

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
}


void MainWindow::showExportDialog()
{
    exportDialog->hardReload();
    exportDialog->exec();
}

void MainWindow::deleteSelectedInList()
{
    QList<QListWidgetItem*> selected = ui->listWidget->selectedItems();
    if (! selected.isEmpty()) {
        for(QListWidgetItem *item: std::as_const(selected)) {
            loeschenAktivitaet(itemToList.value(item));
        }
    }
}

void MainWindow::showPersonal()
{
    personalfenster->show();
    personalfenster->setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    personalfenster->raise();
    personalfenster->activateWindow();
}


void MainWindow::showPreviousMonth()
{
    ui->dateSelector->setDate(ui->dateSelector->date().addDays(-7));
    ui->dateSelector->repaint();
}
void MainWindow::showNextMonth()
{
    ui->dateSelector->setDate(ui->dateSelector->date().addDays(7));
    ui->dateSelector->repaint();
}
void MainWindow::showCurrentMonth()
{
    QDate cur = QDate::currentDate();
    ui->dateSelector->setDate(cur.addDays(-cur.day()+1));
    ui->dateSelector->repaint();
}
void MainWindow::showDate(QDate date)
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

    for (AActivity *a: manager->getActivities()) {
        int pos = getPosInCalendar(a->getDatum());
        if (pos != -1) {
            tage.at(pos)->insert(a);
        }
    }
}


void MainWindow::newFahrtag(QDate d)
{
    Fahrtag *f = manager->newFahrtag(d);
    newAActivityHandler(f);
    openAktivitaet(f);
}
void MainWindow::newActivity(QDate d)
{
    AActivity *a = manager->newActivity(d);
    newAActivityHandler(a);
    openAktivitaet(a);
}
void MainWindow::newAActivityHandler(AActivity *a)
{
    // Einfügen in die Seitenliste
    ui->listWidget->insertItem(ui->listWidget->count(), a->getString());
    QListWidgetItem *i = ui->listWidget->item(ui->listWidget->count()-1);
    listitem.insert(a, i);
    itemToList.insert(i, a);

    onAktivitaetWurdeBearbeitet(a);
}


void MainWindow::setListItem(QListWidgetItem *i, AActivity *a)
{
    if (i == nullptr) return;
    i->setText(a->getString());
    i->setToolTip(toString(a->getArt()));
    i->setBackground(QBrush(QColor(a->getFarbe())));
    i->setForeground(QBrush(QColor("black")));
    QFont font = i->font();
    font.setStrikeOut(a->getAbgesagt());
    font.setBold(a->getWichtig());
    i->setFont(font);
}

int MainWindow::getPosInCalendar(QDate date)
{
    QDate start = ui->dateSelector->date();
//    start = start.addDays(-start.day()+1); // Datum auf Monatsanfang setzen
    start = start.addDays(-start.dayOfWeek()+1);

    long long diff = start.daysTo(date);
    if (diff < 0 || diff >= tage.length()) {
        return -1;
    }
    return static_cast<int>(diff);
}
