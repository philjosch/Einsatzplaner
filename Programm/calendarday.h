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
    void show(QDate datum, QList<AActivity> *objekte);
    void show(QDate datum);
    void setGray(bool gray);

    QListWidgetItem *insert(AActivity *a);

private:
    Ui::CalendarDay *ui;
};

#endif // CALENDARDAY_H
