#include "calendarday.h"
#include "ui_calendarday.h"

CalendarDay::CalendarDay(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CalendarDay)
{
    ui->setupUi(this);
}

CalendarDay::~CalendarDay()
{
    delete ui;
}

void CalendarDay::show(QDate datum, QList<Activity> *objekte)
{
    show(datum);
}
void CalendarDay::show(QDate datum)
{
    ui->label->setText(datum.toString("dd."));
}

void CalendarDay::setGray(bool gray)
{
    if (gray)
        ui->label->setStyleSheet("QLabel {color: #aaa;}");
    else
        ui->label->setStyleSheet("QLabel {color: black;}");
}
