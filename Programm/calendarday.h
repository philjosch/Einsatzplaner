#ifndef CALENDARDAY_H
#define CALENDARDAY_H

#include <QFrame>

namespace Ui {
class CalendarDay;
}

class CalendarDay : public QFrame
{
    Q_OBJECT

public:
    explicit CalendarDay(QWidget *parent = 0);
    ~CalendarDay();

private:
    Ui::CalendarDay *ui;
};

#endif // CALENDARDAY_H
