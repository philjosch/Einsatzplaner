#include "activity.h"
#include <QMap>

Activity::Activity(QDate *date, ManagerPersonal *p): AActivity(date, p)
{
}

Activity::~Activity()
{
    AActivity::~AActivity();
}

QString Activity::getListStringShort() {
    if (anlass == "") {
        return "Aktivität";
    }
    return anlass;
}

QString Activity::getListString()
{
    return datum->toString("dddd dd.MM.yyyy")+" – Aktivität";
}

AActivity::Infos *Activity::getIndividual(Person *person)
{
    Infos *neu = new Infos();
    Infos *alt = personen->value(person);
    neu->aufgabe = alt->aufgabe;
    neu->bemerkung = alt->bemerkung;
    neu->kategorie = alt->kategorie;
    neu->beginn = alt->beginn;
    neu->ende = alt->ende;

    if (alt->beginn == QTime(0,0)) {
        neu->beginn = QTime(zeitAnfang->hour(), zeitAnfang->minute());
    }
    if (alt->ende == QTime(0,0)) {
        neu->ende = QTime(zeitEnde->hour(), zeitEnde->minute());
    }
    return neu;
}

}

void Activity::handleActivity(AActivity *a)
{
    emit activityModified(a);
}

void Activity::handleEmit()
{
    handleActivity(this);
}
