#include "manager.h"

Manager::Manager()
{
    activities = new QList<Activity*>();
}

QJsonObject Manager::toJson()
{

}

void Manager::fromJson(QJsonObject *o)
{

}

Fahrtag *Manager::newFahrtag()
{
    Fahrtag *f = new Fahrtag();
    activities->append(f);
    update(activities->length()-1);
    return f;
}

Activity *Manager::newActivity()
{
    Activity *a = new Activity();
    activities->append(a);
    update(activities->length()-1);
    return a;
}

bool Manager::removeActivity(Activity *a)
{
    return activities->removeOne(a);
}

void Manager::addActivity(Activity *a)
{
    activities->append(a);
    update(activities->length()-1);
}

void Manager::activityChanged(Activity *a)
{
    int pos = activities->indexOf(a);
    if (pos >= 0)
        update(pos);
}

void Manager::update(int pos)
{
    // Verschieb das Objekt an Stelle pos an die Richtige Stelle, sodass später schneller darauf zugegriffen werden kann
    int i = pos;
    while (i < activities->length()-1) {
        if (activities->at(i)->getDatum() >  activities->at(i+1)->getDatum()) {
            activities->swap(i, i+1);
            i++;
        } else {
            break;
        }
    }
    while (i > 0) {
        if (activities->at(i)->getDatum() <  activities->at(i-1)->getDatum()) {
            activities->swap(i, i-1);
            i--;
        } else {
            break;
        }
    }

}
