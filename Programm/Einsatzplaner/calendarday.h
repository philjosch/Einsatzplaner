#ifndef CALENDARDAY_H
#define CALENDARDAY_H

#include "aactivity.h"
#include "activityfiltermodel.h"
#include "activitymodel.h"

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

    void setModel(ActivityModel *sourceModel);

signals:
    void clickedItem(QModelIndex);
    void addActivity(QDate);
private:
    Ui::CalendarDay *ui;
    QDate date;
    ActivityFilterModel *model;
};

#endif // CALENDARDAY_H
