#include "calendarday.h"
#include "ui_calendarday.h"

CalendarDay::CalendarDay(QWidget *parent) : QFrame(parent), ui(new Ui::CalendarDay)
{
    ui->setupUi(this);
    actToItem = QMap<AActivity*, QListWidgetItem*>();
    itemToAct = QMap<QListWidgetItem*, AActivity*>();
    connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(handler(QListWidgetItem*)));
}

CalendarDay::~CalendarDay()
{
    delete ui;
}

void CalendarDay::show(QDate datum)
{
    date = datum;
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
    return actToItem.value(a);
}

bool CalendarDay::remove(AActivity *a)
{
    if (! actToItem.contains(a)) return false;
    QListWidgetItem *item = actToItem.value(a);
    actToItem.remove(a);
    itemToAct.remove(item);
    int row = ui->listWidget->row(item);
    ui->listWidget->takeItem(row);
    delete item;
    return true;
}

QListWidgetItem *CalendarDay::insert(AActivity *a)
{
    QListWidgetItem* item = new QListWidgetItem(a->getListStringShort());
    ui->listWidget->insertItem(ui->listWidget->count(), item);
    actToItem.insert(a, item);
    itemToAct.insert(item, a);
    return item;
}

void CalendarDay::handler(QListWidgetItem *item)
{
    ui->listWidget->setItemSelected(item, false);
    emit clickedItem(itemToAct.value(item));
}

void CalendarDay::on_buttonAdd_clicked()
{
    emit addActivity(date);
}
