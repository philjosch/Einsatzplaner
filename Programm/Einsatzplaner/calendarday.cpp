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
    ui->label->setText(datum.toString("d "));
    if (date.day() == 1)
        ui->label->setText(datum.toString("d. MMM "));
    ui->listWidget->clear();
    actToItem.clear();

    if (datum == QDate::currentDate()) {
        if (gray) {
            ui->buttonAdd->setStyleSheet("border: none; background-color: rgba(255,128,128,0.5); color: #888;");
            ui->label->setStyleSheet("background-color: rgba(255,128,128,0.5); color: #888;");
        } else {
            ui->buttonAdd->setStyleSheet("border: none; background-color: #f00;");
            ui->label->setStyleSheet("background-color: #f00;");
        }
    } else {
        if (gray) {
            ui->buttonAdd->setStyleSheet("border: none; background-color: palette(base); color: #888;");
            ui->label->setStyleSheet("background-color: palette(base); color: #888;");
        } else {
            ui->buttonAdd->setStyleSheet("border: none; background-color: palette(base);");
            ui->label->setStyleSheet("background-color: palette(base);");
        }
    }
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
    item->setToolTip(toString(a->getArt()));
    item->setBackground(QBrush(QColor(a->getFarbe())));
    item->setForeground(QBrush(QColor("black")));
    ui->listWidget->insertItem(ui->listWidget->count(), item);
    actToItem.insert(a, item);
}
