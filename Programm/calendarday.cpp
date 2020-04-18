#include "calendarday.h"
#include "ui_calendarday.h"

CalendarDay::CalendarDay(QWidget *parent) : QFrame(parent), ui(new Ui::CalendarDay)
{
    ui->setupUi(this);
    actToItem = QMap<AActivity*, QListWidgetItem*>();
    connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(handler(QListWidgetItem*)));
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

    if (gray)
        ui->label->setStyleSheet("QLabel {color: #aaa;}");
    else
        ui->label->setStyleSheet("QLabel {color: black;}");
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
    QListWidgetItem* item = new QListWidgetItem(a->getListStringShort());
    item->setBackground(QBrush(QColor(getFarbe(a))));
    ui->listWidget->insertItem(ui->listWidget->count(), item);
    actToItem.insert(a, item);
}

void CalendarDay::handler(QListWidgetItem *item)
{
    item->setSelected(false);
    ui->listWidget->repaint();
    emit clickedItem(actToItem.key(item));
}

void CalendarDay::on_buttonAdd_clicked()
{
    emit addActivity(date);
}
