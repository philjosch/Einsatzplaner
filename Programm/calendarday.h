#ifndef CALENDARDAY_H
#define CALENDARDAY_H

#include <QFrame>
#include <QDate>
#include <QListWidgetItem>

#include "aactivity.h"
#include "fahrtag.h"

namespace Ui {
class CalendarDay;
}

class CalendarDay : public QFrame
{
    Q_OBJECT

public:
    explicit CalendarDay(QWidget *parent = 0);
    ~CalendarDay();
    void show(QDate datum);
    void setGray(bool gray);

    QListWidgetItem *get(AActivity *a);
    bool remove(AActivity *a);

    QListWidgetItem *insert(AActivity *a);
signals:
    void clickedItem(AActivity *);
    void addActivity(QDate);
private slots:
    void handler(QListWidgetItem *item);
    void on_buttonAdd_clicked(bool b);
private:
    Ui::CalendarDay *ui;
    QDate date;
    QMap<AActivity*, QListWidgetItem*> *actToItem;
    QMap<QListWidgetItem*, AActivity*> *itemToAct;
};

#endif // CALENDARDAY_H
