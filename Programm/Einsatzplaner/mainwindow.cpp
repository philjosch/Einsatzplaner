#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "export.h"
#include "exportdialog.h"
#include "fileio.h"
#include "coreapplication.h"
#include "fahrtagwindow.h"
#include "activitywindow.h"

#include <QMessageBox>
#include <QJsonArray>

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
    fenster = QMap<AActivity*, QMainWindow*>();

    connect(ui->actionNeuArbeitseinsatz, SIGNAL(triggered(bool)), this, SLOT(newActivity()));
    connect(ui->actionNeuFahrtag, SIGNAL(triggered(bool)), this, SLOT(newFahrtag()));
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
    this->setGeometry(kalender.x, kalender.y, kalender.width, kalender.height);
    // Personalfenster
    EplFile::FensterPosition personal = datei->getPositionPersonal();
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
    if (datei->getDateiEigenschaften()->getAutom()) {
        int result = Export::Upload::autoUploadToServer(manager->filter(datei->getDateiEigenschaften()->getAuswahl()), datei->getDateiEigenschaften()->getServer());
        if (result == 0)
            ui->statusBar->showMessage(tr("Datei konnte nicht hochgeladen werden!"), 5000);
        else if (result > 0)
            ui->statusBar->showMessage(tr("Datei wurde erfolgreich hochgeladen!"), 5000);
    }
    return;
}

void MainWindow::onAktivitaetWirdEntferntWerden(AActivity *a)
{
    ui->listWidget->takeItem(ui->listWidget->row(listitem.value(a)));
    itemToList.remove(listitem.value(a));
    listitem.remove(a);
    int pos = getPosInCalendar(a->getDatum());
    if (pos >= 0)
        tage.at(pos)->remove(a);
    if (fenster.contains(a)) {
        QMainWindow *w = fenster.value(a);
        fenster.remove(a);
        w->close();
        delete w;
    }
}
void MainWindow::onAktivitaetWurdeBearbeitet(AActivity *a, QDate altesDatum)
{
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


void MainWindow::on_actionExport_triggered()
{
    exportDialog->hardReload();
    exportDialog->exec();
}

void MainWindow::on_actionLoeschen_triggered()
{
    QList<QListWidgetItem*> selected = ui->listWidget->selectedItems();
    if (! selected.isEmpty()) {
        for(QListWidgetItem *item: selected) {
            loeschenAktivitaet(itemToList.value(item));
        }
    }
}

void MainWindow::on_actionPersonal_triggered()
{
    personalfenster->show();
    personalfenster->setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    personalfenster->raise();
    personalfenster->activateWindow();
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


void MainWindow::newFahrtag(QDate d)
{
    Fahrtag *f = manager->newFahrtag(d);
    newAActivityHandler(f);
    openAActivity(f);
}
void MainWindow::newActivity(QDate d)
{
    AActivity *a = manager->newActivity(d);
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
            w = new ActivityWindow(this, a);
        } else {
            Fahrtag* f = dynamic_cast<Fahrtag*>(a);
            w = new FahrtagWindow(this, f);
        }
        w->setWindowFilePath(datei->getPfad());
        fenster.insert(a, w);
        w->show();
    }
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


void MainWindow::onItemInListClicked(QListWidgetItem *item)
{
    openAActivity(itemToList.value(item));
}
void MainWindow::setListItem(QListWidgetItem *i, AActivity *a)
{
    if (i == nullptr) return;
    i->setText(a->getString());
    i->setToolTip(a->getAnlass().replace("<br/>","\n"));
    i->setWhatsThis(a->getAnlass().replace("<br/>","\n"));
    i->setBackground(QBrush(QColor(a->getFarbe())));
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
