#include "calendarday.h"
#include "ui_calendarday.h"

CalendarDay::CalendarDay(QWidget *parent) : QFrame(parent), ui(new Ui::CalendarDay)
{
    ui->setupUi(this);
    connect(ui->listView, &QListView::doubleClicked, this, [=](QModelIndex index) {
        emit clickedItem(model->mapToSource(index));
    });
    connect(ui->buttonAdd, &QToolButton::clicked, this, [=]() { emit addActivity(date); });
}

CalendarDay::~CalendarDay()
{
    delete ui;
}

void CalendarDay::show(QDate datum, bool gray)
{
    date = datum;
    ui->label->setText(datum.toString("d "));
    model->setDateStart(datum.startOfDay());
    model->setDateEnd(date.endOfDay());
    if (date.day() == 1)
        ui->label->setText(QLocale::system().toString(datum, "d. MMM "));

    if (datum == QDate::currentDate()) {
        ui->buttonAdd->setStyleSheet("background-color: #6B1B23; color: white; border: none; ");
        ui->label->setStyleSheet(    "background-color: #6B1B23; color: white;");
        return;
    }
    if (gray) {
        ui->buttonAdd->setStyleSheet("background-color: palette(base); color: #888; border: none; ");
        ui->label->setStyleSheet(    "background-color: palette(base); color: #888;");
        return;
    }
    ui->buttonAdd->setStyleSheet("background-color: palette(base); color: palette(text); border: none; ");
    ui->label->setStyleSheet(    "background-color: palette(base); color: palette(text);");
}

void CalendarDay::setModel(Manager *sourceModel)
{
    model = new ActivityFilterModel();
    model->setSource(sourceModel);
    model->setIgnoreTypes();
    ui->listView->setModel(model);
    ui->listView->setModelColumn(2);
    ui->listView->show();
}
