#include "calendar.h"
#include "ui_calendar.h"

Calendar::Calendar(QWidget *parent) :
    QFrame(parent), Manager(),
    ui(new Ui::Calendar)
{
    ui->setupUi(this);
    connect(ui->buttonAddActivity, SIGNAL(clicked(bool)), this, SLOT(newActivity()));
    connect(ui->buttonAddFahrtag, SIGNAL(clicked(bool)), this, SLOT(newFahrtag()));
    connect(ui->buttonNext, SIGNAL(clicked(bool)), this, SLOT(nextMonth()));
    connect(ui->buttonPrev, SIGNAL(clicked(bool)), this, SLOT(prevMonth()));
    connect(ui->buttonDelete, SIGNAL(clicked(bool)), this, SLOT(removeSelected()));
    connect(ui->buttonToday, SIGNAL(clicked(bool)), this, SLOT(goTo(QDate::currentDate())));
    connect(ui->dateSelector, SIGNAL(dateChanged(QDate)), this, SLOT(goTo(QDate)));

    tage = new QList<CalendarDay*>();
    tage->append(ui->day1_1);
    tage->append(ui->day2_1);
    tage->append(ui->day3_1);
    tage->append(ui->day4_1);
    tage->append(ui->day5_1);
    tage->append(ui->day6_1);
    tage->append(ui->day7_1);
    tage->append(ui->day1_2);
    tage->append(ui->day2_2);
    tage->append(ui->day3_2);
    tage->append(ui->day4_2);
    tage->append(ui->day5_2);
    tage->append(ui->day6_2);
    tage->append(ui->day7_2);
    tage->append(ui->day1_3);
    tage->append(ui->day2_3);
    tage->append(ui->day3_3);
    tage->append(ui->day4_3);
    tage->append(ui->day5_3);
    tage->append(ui->day6_3);
    tage->append(ui->day7_3);
    tage->append(ui->day1_4);
    tage->append(ui->day2_4);
    tage->append(ui->day3_4);
    tage->append(ui->day4_4);
    tage->append(ui->day5_4);
    tage->append(ui->day6_4);
    tage->append(ui->day7_4);
    tage->append(ui->day1_5);
    tage->append(ui->day2_5);
    tage->append(ui->day3_5);
    tage->append(ui->day4_5);
    tage->append(ui->day5_5);
    tage->append(ui->day6_5);
    tage->append(ui->day7_5);

    //    goTo(QDate::currentDate());

}

Calendar::~Calendar()
{
    delete ui;
}

void Calendar::showList(bool visible)
{

}

void Calendar::nextMonth()
{

}

void Calendar::prevMonth()
{

}

void Calendar::goTo(QDate date)
{
    int wochentag = date.dayOfWeek();
    int year = date.year();
    int month = date.month();
    int day = date.day();
    if (date.month() == 1) {
        year -= 1;
        month = 12;
    }
    day = QDate(year, month, 1).daysInMonth();
    day = day-wochentag+1;

    QDate start = QDate(year, month, day);

    for(int i = 0; i < 35; i++) {
        //tage->at(i).showDate(start);
        start = start.addDays(1);
    }
}

bool Calendar::removeSelected()
{
return false;
}

Fahrtag *Calendar::newFahrtag()
{
    Fahrtag *f = Manager::newFahrtag();
    // Hier muss Implementiert werden
    emit showFahrtag(f);
    return f;
}

Activity *Calendar::newActivity()
{
    Activity *a = Manager::newActivity();
    // Hier muss die Anbindung an die GUI implementiert werden
    emit showActivity(a);
    return a;
}
