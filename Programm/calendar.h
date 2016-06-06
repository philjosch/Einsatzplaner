#ifndef CALENDAR_H
#define CALENDAR_H

#include <QFrame>
#include <QDate>
#include "manager.h"
#include "activity.h"
#include "fahrtag.h"

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
    void showList(bool visible);
    void nextMonth();
    void prevMonth();
    void goTo(QDate date);
    bool removeSelected();

    Fahrtag *newFahrtag();
    Activity *newActivity();

signals:
    void showFahrtag(Fahrtag*);
    void showActivity(Activity*);

private:
    Ui::Calendar *ui;
};

#endif // CALENDAR_H
