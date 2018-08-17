#include "calendar.h"
#include "ui_calendar.h"
#include "mainwindow.h"

#include <QMessageBox>
#include <QDebug>

Calendar::Calendar(QWidget *parent) : QFrame(parent), Manager(), ui(new Ui::Calendar)
{
    ui->setupUi(this);

    listitem = new QMap<AActivity*, QListWidgetItem*>();
    itemToList = new QMap<QListWidgetItem*, AActivity*>();
    calendaritem = new QMap<AActivity*, CalendarDay*>();

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
        connect(c, SIGNAL(clickedItem(AActivity*)), this, SLOT(clickedItemCalendar(AActivity*)));
        connect(c, SIGNAL(addActivity(QDate)), this, SLOT(newActivity(QDate)));
    }
    goToday();

    scrollOffSet = 0;
}

Calendar::~Calendar()
{
    delete ui;
}

QJsonObject Calendar::toJson()
{
    QJsonObject o = Manager::toJson();
    o.insert("currentDate", ui->dateSelector->date().toString("yyyy-MM-dd"));
    return o;
}

QJsonObject Calendar::personalToJson()
{
    QJsonObject o = Manager::personalToJson();
    o.insert("currentDate", ui->dateSelector->date().toString("yyyy-MM-dd"));
    return o;
}

void Calendar::fromJson(QJsonObject o)
{
    // Daten in den Manager laden und die ogik herstellen
    Manager::fromJson(o);
    // Alle aktivitäten in die seitenleiste eintragen
    for(AActivity *a: *activities) {
        QListWidgetItem *i = new QListWidgetItem("");
        if (Fahrtag *f = dynamic_cast<Fahrtag*>(a)) {
            connect(f, SIGNAL(changed(AActivity*)), this, SLOT(activityChanged(AActivity*)));
        } else {
            Activity *a_cast = dynamic_cast<Activity*>(a);
            connect(a_cast, SIGNAL(changed(AActivity*)), this, SLOT(activityChanged(AActivity*)));
        }
        setListItem(i, a);
        ui->listWidget->insertItem(ui->listWidget->count(), i);
        listitem->insert(a, i);
        itemToList->insert(i, a);
    }
    // an das gespeicherte Datum gehen
    ui->dateSelector->setDate(QDate::fromString(o.value("currentDate").toString(), "yyyy-MM-dd"));
    ui->dateSelector->repaint();
    goTo(ui->dateSelector->date());
}

void Calendar::wheelEvent(QWheelEvent *event)
{
//    int scrollAmount = 30;
//    scrollOffSet += event->angleDelta().y();
//    ui->dateSelector->setDate(ui->dateSelector->date().addDays(- scrollOffSet / scrollAmount));
//    scrollOffSet %= scrollAmount;
}

void Calendar::nextMonth()
{
    ui->dateSelector->setDate(ui->dateSelector->date().addMonths(1));
    ui->dateSelector->repaint();
}

void Calendar::prevMonth()
{
    ui->dateSelector->setDate(ui->dateSelector->date().addMonths(-1));
    ui->dateSelector->repaint();
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
    for(int i = 0; i < activities->length(); i++) {
        AActivity *a = activities->at(i);
        int pos = getPosInCalendar(a->getDatum());
        if (pos != -1) {
            tage->at(pos)->insert(a);
            calendaritem->insert(a, tage->at(pos));
            setListItemC(calendaritem->value(a)->get(a), a);
        }
    }
}

void Calendar::goToday()
{
    ui->dateSelector->setDate(QDate::currentDate());
    ui->dateSelector->repaint();
}

bool Calendar::removeSelected()
{
    QList<QListWidgetItem*> selected = ui->listWidget->selectedItems();
    for(QListWidgetItem *item: selected) {
        removeActivity(itemToList->value(item));
    }
    return false;
}

Fahrtag *Calendar::newFahrtag(QDate d)
{
    // Anlegen des Fahrtags
    Fahrtag *f = Manager::newFahrtag(d);
    connect(f, SIGNAL(changed(AActivity*)), this, SLOT(activityChanged(AActivity*)));

    // Einfügen in Seitenliste und Kalender
    insert(f);
    activityChanged(f);

    // Anzeigen eines neuen Fensters
    emit showFahrtag(f);
    emit changed();
    return f;
}

Activity *Calendar::newActivity(QDate d)
{
    // Anlegen der Aktivität
    Activity *a = Manager::newActivity(d);
    connect(a, SIGNAL(changed(AActivity*)), this, SLOT(activityChanged(AActivity*)));

    // Einfügen in die Seitenleiste
    insert(a);
    activityChanged(a);

    // Anzeigen einen neuen Fensters
    emit showActivity(a);
    emit changed();
    return a;
}

bool Calendar::removeActivity(AActivity *a)
{
    ui->listWidget->takeItem(ui->listWidget->row(listitem->value(a)));
    itemToList->remove(listitem->value(a));
    listitem->remove(a);
    calendaritem->value(a)->remove(a);
    calendaritem->remove(a);
    Manager::removeActivity(a);
    emit changed();
    return true;
}

void Calendar::activityChanged(AActivity *a)
{
    Manager::activityChanged(a);

    // Richtiges Anzeigen im Kalender
    int pos = getPosInCalendar(a->getDatum());
    if (pos < 42 && pos >= 0) {
        /* Element befindet sich nach Aktualiserung im Kalender
         * -> Befindet sich schon jetzt im Kalender
         * ---> Position verändert
         * ---> Position nciht verändert
         * -> Befindet sich im Moment nicht im Kalender
         * */
        if (calendaritem->contains(a)) {
            // Element ist bereits im Kalender
            CalendarDay *pos_neu = tage->at(pos);
            CalendarDay *pos_alt = calendaritem->value(a);
            if ( ! (pos_neu == pos_alt)) {
                pos_alt->remove(a);
                pos_neu->insert(a);
                calendaritem->insert(a, pos_neu);
            }
        } else {
            // Element ist nciht im Kalender und muss eingefügt werden
            CalendarDay *pos_neu = tage->at(pos);
            calendaritem->insert(a, pos_neu);
            pos_neu->insert(a);
        }
        setListItemC(calendaritem->value(a)->get(a), a);
    } else {
        // Element befindet sich nicht im Kalender
        if (calendaritem->contains(a)) {
            calendaritem->value(a)->remove(a);
            calendaritem->remove(a);
        }
    }

    // Richtiges Anzeigen in der Übersichts liste
    setListItem(listitem->value(a), a);

    // Prüfen, ob das element weitgenug vorne (oben) ist
    int i = ui->listWidget->row(listitem->value(a));
    AActivity *ref;
    while (i > 0) {
        ref = itemToList->value(ui->listWidget->item(i-1));
        if (ref->operator>(a)) {
            ui->listWidget->insertItem(i-1, ui->listWidget->takeItem(i));
            i--;
        } else {
            break;
        }
    }

    // Prüfen, ob das element weit genug hinten (unten) ist
    while (i < ui->listWidget->count()-1) {
        ref = itemToList->value(ui->listWidget->item(i+1));
        if (ref->operator<(a)) {
            ui->listWidget->insertItem(i, ui->listWidget->takeItem(i+1));
            i++;
        } else {
            break;
        }
    }
    emit changed();
}

void Calendar::clickedItem(QListWidgetItem *i)
{
    AActivity *a = itemToList->value(i);
    if (Fahrtag *f = dynamic_cast<Fahrtag*>(a)) {
        emit showFahrtag(f);
    } else {
        Activity *c = dynamic_cast<Activity*>(a);
        emit showActivity(c);
    }
}

void Calendar::clickedItemCalendar(AActivity *a)
{
    if (Fahrtag *f = dynamic_cast<Fahrtag*>(a)) {
        emit showFahrtag(f);
    } else {
        Activity *c = dynamic_cast<Activity*>(a);
        emit showActivity(c);
    }
}

int Calendar::getItemFromDate(QDate date)
{
    int year = ui->dateSelector->date().year();
    int month = ui->dateSelector->date().month();
    int wochentag = QDate(year, month, 1).dayOfWeek();

    return wochentag - 1 + date.day();
}

int Calendar::getPosInCalendar(QDate date)
{
    QDate ref = QDate(ui->dateSelector->date().year(), ui->dateSelector->date().month(), 1);
    // Eintrag auch anzeigen, wenn er im vorherigen Monat liegt
    if (date.month() == ref.month() && date.year() == ref.year()) {
        return getItemFromDate(date)-1;
    } else if (date.addMonths(1).month() == ref.month() && date.addMonths(1).year() == ref.year()) {
        if (date.addDays(7) > ref && date.dayOfWeek() < ref.dayOfWeek()) {
            return date.dayOfWeek() - 1;
        }
        return -1;
    } else if (date.month() == ref.addMonths(1).month() && date.year() == ref.addMonths(1).year()) {
        if (date.day()+ref.dayOfWeek()+ref.daysInMonth()-2 < 42) {
            return date.day()+ref.dayOfWeek()+ref.daysInMonth()-2;
        } else {
            return -1;
        }
    } else {
        return -1;
    }
}

void Calendar::insert(AActivity *a)
{
    // Einfügen in die Seitenliste
    ui->listWidget->insertItem(ui->listWidget->count(), a->getListString());
    QListWidgetItem *i = ui->listWidget->item(ui->listWidget->count()-1);
    listitem->insert(a, i);
    itemToList->insert(i, a);

    // Einfügen in den Kalender mit allem drum und dran
    int pos = getPosInCalendar(a->getDatum());
    if (pos != -1) {
        tage->at(pos)->insert(a);
        calendaritem->insert(a, tage->at(pos));
    }
}

void Calendar::setListItemC(QListWidgetItem *i, AActivity *a)
{
    if (i == nullptr) return;
    i->setText(a->getListStringShort());
    i->setBackgroundColor(MainWindow::getFarbe(a));
}

void Calendar::setListItem(QListWidgetItem *i, AActivity *a)
{
    if (i == nullptr) return;
    i->setText(a->getListString());
    i->setToolTip(a->getAnlass());
    i->setWhatsThis(a->getAnlass());
    i->setBackgroundColor(MainWindow::getFarbe(a));
}
