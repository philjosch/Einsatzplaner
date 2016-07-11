#ifndef CALENDAR_H
#define CALENDAR_H

#include <QFrame>
#include <QDate>
#include <QListWidgetItem>
#include "manager.h"
#include "activity.h"
#include "fahrtag.h"
#include "calendarday.h"

namespace Ui {
class Calendar;
}

class Calendar : public QFrame, Manager
{
    Q_OBJECT

public:
    explicit Calendar(QWidget *parent = 0);
    ~Calendar();

public slots:
    void nextMonth();
    void prevMonth();
    void goTo(QDate date);
    void goToday();
    bool removeSelected();

    Fahrtag *newFahrtag();
    Activity *newActivity();
    bool removeActivity(AActivity *a);

    void activityChanged(AActivity *a);

    void clickedItem(QListWidgetItem *i);

signals:
    void showFahrtag(Fahrtag*);
    void showActivity(Activity*);

private:
    int getItemFromDate(QDate *date);
    Ui::Calendar *ui;
    QList<CalendarDay*> *tage;

    QMap<AActivity*, QListWidgetItem*> *calendaritem;
    QMap<AActivity*, QListWidgetItem*> *listitem;
};

#endif // CALENDAR_H
