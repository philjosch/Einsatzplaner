#include "calendarday.h"
#include "ui_calendarday.h"

CalendarDay::CalendarDay(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CalendarDay)
{
    ui->setupUi(this);
    liste = new QMap<AActivity*, QListWidgetItem*>();
    connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(handler(QListWidgetItem*)));
}

CalendarDay::~CalendarDay()
{
    delete ui;
}

void CalendarDay::show(QDate datum, QList<AActivity> *objekte)
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

QListWidgetItem *CalendarDay::get(AActivity *a)
{
    return liste->value(a);
}

bool CalendarDay::remove(AActivity *a)
{

    ui->listWidget->takeItem(ui->listWidget->row(liste->value(a)));
    liste->remove(a);

    return true;

}

QListWidgetItem *CalendarDay::insert(AActivity *a)
{
    ui->listWidget->insertItem(0, a->getListStringShort());
    QListWidgetItem* item = ui->listWidget->item(0);
    liste->insert(a, item);
    return item;
}

void CalendarDay::handler(QListWidgetItem *a)
{
    emit clickedItem(a);
}
