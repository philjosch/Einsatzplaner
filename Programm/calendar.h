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

class Calendar : public QFrame, public Manager
{
    Q_OBJECT

public:
    explicit Calendar(QWidget *parent = 0);
    ~Calendar();

    QJsonObject toJson();
    QJsonObject personalToJson();
    void fromJson(QJsonObject o);

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
    void clickedItemCalendar(AActivity *a);

signals:
    void showFahrtag(Fahrtag*);
    void showActivity(Activity*);

    void changed();

protected:
    int getItemFromDate(QDate date);
    int getPosInCalendar(QDate date);
    void insert(AActivity *a);
    void setListItemC(QListWidgetItem *i, AActivity *a);
    void setListItem(QListWidgetItem *i, AActivity *a);
    Ui::Calendar *ui;
    QList<CalendarDay*> *tage;

    QMap<AActivity*, CalendarDay*> *calendaritem; // Gibt an, in welchem Tag die Elemente momentan angezeigt werden

    QMap<AActivity*, QListWidgetItem*> *listitem; // Gibt an, welcher Aktivität welches Listenelement in der gesamtListe zugeordnet ist
    QMap<QListWidgetItem*, AActivity*> *itemToList;
};

#endif // CALENDAR_H
