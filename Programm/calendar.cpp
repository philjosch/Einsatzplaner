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

    goTo(QDate::currentDate());
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

}

bool Calendar::removeSelected()
{

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
