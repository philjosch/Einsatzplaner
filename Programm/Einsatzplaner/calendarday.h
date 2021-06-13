#ifndef CALENDARDAY_H
#define CALENDARDAY_H

#include "aactivity.h"

#include <QListWidgetItem>

namespace Ui {
class CalendarDay;
}

class CalendarDay : public QFrame
{
    Q_OBJECT

public:
    explicit CalendarDay(QWidget *parent);
    ~CalendarDay();
    void show(QDate datum, bool gray);
    void remove(AActivity *a);
    void insert(AActivity *a);
signals:
    void clickedItem(AActivity *);
    void addActivity(QDate);
private:
    Ui::CalendarDay *ui;
    QDate date;
    QMap<AActivity*, QListWidgetItem*> actToItem;
};

#endif // CALENDARDAY_H
