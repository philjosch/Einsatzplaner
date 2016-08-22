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

QList<int> *Activity::getIndividual(Person *person)
{
    QList<int> *l = new QList<int>();
    QList<int> *orig = personen->value(person);
    if (orig->at(0) == 0) {
        l->append(zeitAnfang->msecsSinceStartOfDay());
    } else {
        l->append(orig->at(0));
    }
    if (orig->at(1) == 0) {
        l->append(zeitEnde->msecsSinceStartOfDay());
    } else {
        l->append(orig->at(1));
    }
    l->append(orig->at(2));

    return l;
}

void Activity::handleActivity(AActivity *a)
{
    emit activityModified(a);
}

void Activity::handleEmit()
{
    handleActivity(this);
}
