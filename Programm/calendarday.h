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
//    void show(QDate datum, QList<AActivity> *objekte);
    void show(QDate datum);
    void setGray(bool gray);

    QListWidgetItem *get(AActivity *a);
    bool remove(AActivity *a);

    QListWidgetItem *insert(AActivity *a);
signals:
    void clickedItem(AActivity *);
private slots:
    void handler(QListWidgetItem *item);
private:
    Ui::CalendarDay *ui;
    QMap<AActivity*, QListWidgetItem*> *actToItem;
    QMap<QListWidgetItem*, AActivity*> *itemToAct;
};

#endif // CALENDARDAY_H
