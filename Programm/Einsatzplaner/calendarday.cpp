#include "calendarday.h"
#include "ui_calendarday.h"

CalendarDay::CalendarDay(QWidget *parent) : QFrame(parent), ui(new Ui::CalendarDay)
{
    ui->setupUi(this);
    actToItem = QMap<AActivity*, QListWidgetItem*>();
    connect(ui->listWidget, &QListWidget::itemDoubleClicked, this, [=](QListWidgetItem *item) {emit clickedItem(actToItem.key(item)); });
    connect(ui->buttonAdd, &QToolButton::clicked, this, [=]() { emit addActivity(date); });
}

CalendarDay::~CalendarDay()
{
    delete ui;
}

void CalendarDay::show(QDate datum, bool gray)
{
    date = datum;
    ui->label->setText(datum.toString("dd.  "));
    ui->listWidget->clear();
    actToItem.clear();

    ui->label->setEnabled(! gray);
}

void CalendarDay::remove(AActivity *a)
{
    if (! actToItem.contains(a)) return;
    QListWidgetItem *item = actToItem.value(a);
    actToItem.remove(a);
    int row = ui->listWidget->row(item);
    ui->listWidget->takeItem(row);
    delete item;
}

void CalendarDay::insert(AActivity *a)
{
    if (actToItem.contains(a)) {
        remove(a);
    }
    QListWidgetItem* item = new QListWidgetItem(a->getStringShort().replace("<br/>","\n"));
    item->setBackground(QBrush(QColor(a->getFarbe())));
    item->setForeground(QBrush(QColor("black")));
    ui->listWidget->insertItem(ui->listWidget->count(), item);
    actToItem.insert(a, item);
}
