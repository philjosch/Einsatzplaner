#include "calendar.h"
#include "ui_calendar.h"
#include <QMessageBox>
#include "mainwindow.h"

Calendar::Calendar(QWidget *parent) :
    QFrame(parent), Manager(),
    ui(new Ui::Calendar)
{
    ui->setupUi(this);

    listitem = new QMap<AActivity*, QListWidgetItem*>();
    calendaritem = new QMap<AActivity*, CalendarDay*>();
    calendarEntries = new QMap<QListWidgetItem*, AActivity*>();

    connect(ui->buttonAddActivity, SIGNAL(clicked(bool)), this, SLOT(newActivity()));
    connect(ui->buttonAddFahrtag, SIGNAL(clicked(bool)), this, SLOT(newFahrtag()));
    connect(ui->buttonNext, SIGNAL(clicked(bool)), this, SLOT(nextMonth()));
    connect(ui->buttonPrev, SIGNAL(clicked(bool)), this, SLOT(prevMonth()));
    connect(ui->buttonDelete, SIGNAL(clicked(bool)), this, SLOT(removeSelected()));
    connect(ui->buttonToday, SIGNAL(clicked(bool)), this, SLOT(goToday()));
    connect(ui->dateSelector, SIGNAL(dateChanged(QDate)), this, SLOT(goTo(QDate)));

    connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(clickedItem(QListWidgetItem*)));

    // Setup fuer die Darstellung des Kalenders
    tage = new QList<CalendarDay*>();
    tage->append(ui->day1_1);    tage->append(ui->day2_1);    tage->append(ui->day3_1);    tage->append(ui->day4_1);
    tage->append(ui->day5_1);    tage->append(ui->day6_1);    tage->append(ui->day7_1);
    tage->append(ui->day1_2);    tage->append(ui->day2_2);    tage->append(ui->day3_2);    tage->append(ui->day4_2);
    tage->append(ui->day5_2);    tage->append(ui->day6_2);    tage->append(ui->day7_2);
    tage->append(ui->day1_3);    tage->append(ui->day2_3);    tage->append(ui->day3_3);    tage->append(ui->day4_3);
    tage->append(ui->day5_3);    tage->append(ui->day6_3);    tage->append(ui->day7_3);
    tage->append(ui->day1_4);    tage->append(ui->day2_4);    tage->append(ui->day3_4);    tage->append(ui->day4_4);
    tage->append(ui->day5_4);    tage->append(ui->day6_4);    tage->append(ui->day7_4);
    tage->append(ui->day1_5);    tage->append(ui->day2_5);    tage->append(ui->day3_5);    tage->append(ui->day4_5);
    tage->append(ui->day5_5);    tage->append(ui->day6_5);    tage->append(ui->day7_5);
    tage->append(ui->day1_6);    tage->append(ui->day2_6);    tage->append(ui->day3_6);    tage->append(ui->day4_6);
    tage->append(ui->day5_6);    tage->append(ui->day6_6);    tage->append(ui->day7_6);
    for(CalendarDay *c: *tage) {
        connect(c, SIGNAL(clickedItem(QListWidgetItem*)), this, SLOT(clickedItemCalendar(QListWidgetItem*)));
    }
    goToday();

}

Calendar::~Calendar()
{
    delete ui;
}

void Calendar::nextMonth()
{
    ui->dateSelector->setDate(ui->dateSelector->date().addMonths(1));
}

void Calendar::prevMonth()
{
    ui->dateSelector->setDate(ui->dateSelector->date().addMonths(-1));
}

void Calendar::goTo(QDate date)
{
    int year = date.year();
    int month = date.month();
    int day;
    int wochentag = QDate(year, month, 1).dayOfWeek();
    if (date.month() == 1) {
        year -= 1;
        month = 12;
    } else {
        month -= 1;
    }
    day = QDate(year, month, 1).daysInMonth();
    QDate start = QDate(year, month, day).addDays(2-wochentag);

    // Eintragen der Wochennummern
    ui->number1_1->setText(QString::number(start.addDays(0).weekNumber()));
    ui->number1_2->setText(QString::number(start.addDays(7).weekNumber()));
    ui->number1_3->setText(QString::number(start.addDays(14).weekNumber()));
    ui->number1_4->setText(QString::number(start.addDays(21).weekNumber()));
    ui->number1_5->setText(QString::number(start.addDays(28).weekNumber()));
    ui->number1_6->setText(QString::number(start.addDays(35).weekNumber()));


    // Einstellen der einzelnen Tage
    for (int i = 0; i < wochentag-1; i++) {
        tage->at(i)->show(start);
        tage->at(i)->setGray(true);
        start = start.addDays(1);
    }
    day = start.daysInMonth();

    for(int i = wochentag-1; i < (wochentag + day - 1); i++) {
        tage->at(i)->show(start);
        tage->at(i)->setGray(false);
        start = start.addDays(1);
    }
    for(int i = (wochentag + day - 1); i < 42; i++) {
        tage->at(i)->show(start);
        tage->at(i)->setGray(true);
        start = start.addDays(1);
    }

    for(int i = 0; i < calendaritem->keys().length(); i++) {
        calendaritem->value(calendaritem->keys().at(i))->remove(calendaritem->keys().at(i));
    }
    calendarEntries->clear();
    for(int i = 0; i < activities->length(); i++) {
        AActivity *a = activities->at(i);
        int pos = getPosInCalendar(a->getDatum());
        if (pos != -1) {
            calendarEntries->insert(tage->at(pos)->insert(a), a);
            calendaritem->insert(a, tage->at(pos));
            activityChanged(a);
//        } else if (calendaritem->contains(a)) {
  //          calendaritem->remove(a);
        }
    }
}

void Calendar::goToday()
{
    ui->dateSelector->setDate(QDate::currentDate());
}

bool Calendar::removeSelected()
{
    QMessageBox::information(this, "Fehler", "Noch nicht implementiert");
    return false;
}

Fahrtag *Calendar::newFahrtag()
{
    // Anlegen des Fahrtags
    QDate d = QDate::currentDate();
    Fahrtag *f = Manager::newFahrtag(&d);
    connect(f, SIGNAL(fahrtagModified(AActivity*)), this, SLOT(activityChanged(AActivity*)));

    // Einfügen in Seitenliste und Kalender
    insert(f);

    // Anzeigen eines neuen Fensters
    emit showFahrtag(f);
    return f;
}

Activity *Calendar::newActivity()
{
    // Anlegen der Aktivität
    QDate d = QDate::currentDate();
    Activity *a = Manager::newActivity(&d);
    connect(a, SIGNAL(activityModified(AActivity*)), this, SLOT(activityChanged(AActivity*)));

    // Einfügen in die Seitenleiste
    insert(a);

    // Anzeigen einen neuen Fensters
    emit showActivity(a);
    return a;
}

bool Calendar::removeActivity(AActivity *a)
{
    QMessageBox::information(this, "Fehler", "Diese Funktion kann im MOment nicht genutzt werden!\nWir bitten um Entschuldigung!");
    return false;
}

void Calendar::activityChanged(AActivity *a)
{
    int pos = getPosInCalendar(a->getDatum());
    if (pos < 42 && pos >= 0) {
        /* Element befindet sich nach Aktualiserung im Kalender
         * -> Befindet sich schon jetzt im Kalender
         * ---> Position veröndert
         * ---> Position nciht verändert
         * -> Befindet sich im Moment nicht im Kalender
         * */
        if (calendaritem->contains(a)) {
            // Element ist bereits im Kalender
            CalendarDay *pos_neu = tage->at(pos);
            CalendarDay *pos_alt = calendaritem->value(a);
            if ( ! (pos_neu == pos_alt)) {
                calendaritem->value(a)->remove(a);
                calendaritem->insert(a, pos_neu);
                calendarEntries->insert(pos_neu->insert(a), a);
            }
        } else {
            // Element ist nciht im Kalender und muss eingefügt werden
            CalendarDay *pos_neu = tage->at(pos);
            calendaritem->insert(a, pos_neu);
            calendarEntries->insert(pos_neu->insert(a), a);
        }
        setListItemC(calendaritem->value(a)->get(a), a);
    } else {
        // Element befindet sich nicht im Kalender
        if (calendaritem->contains(a)) {
            calendaritem->value(a)->remove(a);
            calendaritem->remove(a);
        }
    }

    // Richtiges positionieren des elementes in der Übersichts liste
    setListItem(listitem->value(a), a);
/*    int i = activities->indexOf(a);
    while (i > 0) {
        QMessageBox::information(this, "", activities->at(i-1)->getDatum()->toString("dd.MM.yyyy"));
        QMessageBox::information(this, "", activities->at(i)->getDatum()->toString("dd.MM.yyyy"));
        if (activities->at(i-1)->getDatum() > activities->at(i)->getDatum()) {
            QMessageBox::information(this, "", "bla bla bla");
        }
        break;
    }

    while ((i > 0) && (activities->at(i-1)->getDatum() > activities->at(i)->getDatum())) {
        activities->swap(i-1, i);
        ui->listWidget->insertItem(i, ui->listWidget->takeItem(i-1));
        ui->listWidget->insertItem(i-1, ui->listWidget->takeItem(i));
        i--;
    }


    while ((i < activities->length()-1) && (activities->at(i)->getDatum() > activities->at(i+1)->getDatum())) {
        activities->swap(i, i+1);
        ui->listWidget->insertItem(i+1, ui->listWidget->takeItem(i));
        ui->listWidget->insertItem(i, ui->listWidget->takeItem(i+1));
        i++;
    }
    */
}

void Calendar::clickedItem(QListWidgetItem *i)
{
    AActivity *a = listitem->key(i, new Fahrtag(new QDate(), nullptr));
    if (Fahrtag *f = dynamic_cast<Fahrtag*>(a)) {
        emit showFahrtag(f);
    } else {
        Activity *c = dynamic_cast<Activity*>(a);
        emit showActivity(c);
    }
}

void Calendar::clickedItemCalendar(QListWidgetItem *i)
{
    AActivity *a = calendarEntries->value(i);
    if (Fahrtag *f = dynamic_cast<Fahrtag*>(a)) {
        emit showFahrtag(f);
    } else {
        Activity *c = dynamic_cast<Activity*>(a);
        emit showActivity(c);
    }
    // Element liegt im Kalender

}

int Calendar::getItemFromDate(QDate *date)
{
    int year = ui->dateSelector->date().year();
    int month = ui->dateSelector->date().month();
    int wochentag = QDate(year, month, 1).dayOfWeek();

    return wochentag -1 + date->day();
}

int Calendar::getPosInCalendar(QDate *date)
{
    QDate ref = QDate(ui->dateSelector->date().year(), ui->dateSelector->date().month(), 1);
    // Eintrag auch anzeigen, wenn er im vorherigen Monat liegt
    if (*date < ref) {
        return -1;
    } else if (date->month() == ref.month()+1) {
        if (date->day()+ref.dayOfWeek()+ref.daysInMonth()-2 < 42) {
            return date->day()+ref.dayOfWeek()+ref.daysInMonth()-2;
        } else {
            return -1;
        }
    } else if (*date > ref.addMonths(1)) {
        return -1;
    } else {
        return getItemFromDate(date)-1;
    }
}

void Calendar::insert(AActivity *a)
{
    // Einfügen in die Seitenliste
    ui->listWidget->insertItem(ui->listWidget->count(), a->getListString());
    QListWidgetItem *i = ui->listWidget->item(ui->listWidget->count()-1);
    listitem->insert(a, i);

    // Einfügen in den Kalender mit allem drum und dran
    int pos = getPosInCalendar(a->getDatum());
    if (pos != -1) {
        QListWidgetItem *i = tage->at(pos)->insert(a);
        calendarEntries->insert(i, a);
        calendaritem->insert(a, tage->at(pos));
    }
}

void Calendar::setListItemC(QListWidgetItem *i, AActivity *a)
{
    i->setText(a->getListStringShort());
    if (Fahrtag *f = dynamic_cast<Fahrtag*>(a)) {
        i->setBackgroundColor(MainWindow::getFarbeZug(f->getArt()));
    } else {
        i->setBackgroundColor(MainWindow::getFarbeArbeit());
    }
}

void Calendar::setListItem(QListWidgetItem *i, AActivity *a)
{
    i->setText(a->getListString());
    if (Fahrtag *f = dynamic_cast<Fahrtag*>(a)) {
        i->setBackgroundColor(MainWindow::getFarbeZug(f->getArt()));
    } else {
        i->setBackgroundColor(MainWindow::getFarbeArbeit());
    }
}
