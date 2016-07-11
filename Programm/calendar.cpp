#include "calendar.h"
#include "ui_calendar.h"
#include <QMessageBox>

Calendar::Calendar(QWidget *parent) :
    QFrame(parent), Manager(),
    ui(new Ui::Calendar)
{
    ui->setupUi(this);

    listitem = new QMap<AActivity*, QListWidgetItem*>();
    calendaritem = new QMap<AActivity*, QListWidgetItem*>();

    connect(ui->buttonAddActivity, SIGNAL(clicked(bool)), this, SLOT(newActivity()));
    connect(ui->buttonAddFahrtag, SIGNAL(clicked(bool)), this, SLOT(newFahrtag()));
    connect(ui->buttonNext, SIGNAL(clicked(bool)), this, SLOT(nextMonth()));
    connect(ui->buttonPrev, SIGNAL(clicked(bool)), this, SLOT(prevMonth()));
    connect(ui->buttonDelete, SIGNAL(clicked(bool)), this, SLOT(removeSelected()));
    connect(ui->buttonToday, SIGNAL(clicked(bool)), this, SLOT(goToday()));
    connect(ui->dateSelector, SIGNAL(dateChanged(QDate)), this, SLOT(goTo(QDate)));

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
}

void Calendar::goToday()
{
    ui->dateSelector->setDate(QDate::currentDate());
}

bool Calendar::removeSelected()
{
    return false;
}

Fahrtag *Calendar::newFahrtag()
{
    Fahrtag *f = Manager::newFahrtag(new QDate());
    connect(f, SIGNAL(fahrtagModified(AActivity*)), this, SLOT(activityChanged(AActivity*)));
    ui->listWidget->insertItem(0, f->getListString());
    QListWidgetItem *i = ui->listWidget->item(0);
    connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(clickedItem(QListWidgetItem*)));
    listitem->insert(f, ui->listWidget->item(0));
    int pos = getItemFromDate(f->getDatum());
    calendaritem->insert(f, tage->at(pos)->insert(f));

    // Hier muss Implementiert werden
    emit showFahrtag(f);
    return f;
}

Activity *Calendar::newActivity()
{
    Activity *a = Manager::newActivity(new QDate());
    connect(a, SIGNAL(activityModified(AActivity*)), this, SLOT(activityChanged(AActivity*)));
    // Hier muss die Anbindung an die GUI implementiert werden
    emit showActivity(a);
    return a;
}

bool Calendar::removeActivity(AActivity *a)
{

}

void Calendar::activityChanged(AActivity *a)
{
//    QMessageBox::information(this, "test", "test", QMessageBox::Ok);
    calendaritem->value(a)->setText(a->getListStringShort());
    listitem->value(a)->setText(a->getListString());
}

void Calendar::clickedItem(QListWidgetItem *i)
{
    if (calendaritem->values().contains(i)) {
        AActivity *a = calendaritem->key(i);
        if (Fahrtag *f = dynamic_cast<Fahrtag*>(a)) {
            emit showFahrtag(f);
        } else {
            Activity *c = dynamic_cast<Activity*>(a);
            emit showActivity(c);
        }
        // Element liegt im Kalender
    } else {
        AActivity *a = listitem->key(i);
        if (Fahrtag *f = dynamic_cast<Fahrtag*>(a)) {
            emit showFahrtag(f);
        } else {
            Activity *c = dynamic_cast<Activity*>(a);
            emit showActivity(c);
        }
    }
}

int Calendar::getItemFromDate(QDate *date)
{
    int year = ui->dateSelector->date().year();
    int month = ui->dateSelector->date().month();
    int wochentag = QDate(year, month, 1).dayOfWeek();

    return wochentag -1 + date->day();
}
