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
