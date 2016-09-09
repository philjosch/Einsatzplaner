#include "person.h"
#include <QMap>
#include <QListIterator>
#include <QDebug>

Person::Person(QString name) : QObject()
{
    this->name = name;
    strecke = 0;
    this->ausbildungTf = false;
    this->ausbildungZf = false;
    this->ausbildungRangierer = false;
    timeTf = 0.f;
    timeZf = 0.f;
    timeZub = 0.f;
    timeService = 0.f;
    timeBuero = 0.f;
    timeWerkstatt = 0.f;
    timeSum = 0.f;
    sumKilometer = 0.f;
    valuesInvalid = true;

    activities = new QMap<AActivity *, AActivity::Category>();
}

bool Person::getAusbildungTf() const
{
    return ausbildungTf;
}

void Person::setAusbildungTf(bool value)
{
    ausbildungTf = value;
}

bool Person::getAusbildungZf() const
{
    return ausbildungZf;
}

void Person::setAusbildungZf(bool value)
{
    ausbildungZf = value;
}

bool Person::getAusbildungRangierer() const
{
    return ausbildungRangierer;
}

void Person::setAusbildungRangierer(bool value)
{
    ausbildungRangierer = value;
}

int Person::getStrecke() const
{
    return strecke;
}

void Person::setStrecke(int value)
{
    strecke = value;
}


void Person::berechne()
{
    timeTf = 0;
    timeZf = 0;
    timeZub = 0;
    timeService = 0;
    timeBuero = 0;
    timeWerkstatt = 0;
    timeSum = 0;

    for(AActivity *a: activities->keys()) {
        AActivity::Category cat = activities->value(a);
        AActivity::Infos *info = a->getIndividual(this);

        // Einsatzstunden
        QTime start = info->beginn;
        QTime ende = info->ende;
        int duration = start.msecsTo(ende);
        switch (cat) {
        case AActivity::Tf: timeTf += duration;  break;
        case AActivity::Zf: timeZf += duration;  break;
        case AActivity::Begleiter: timeZub += duration;  break;
        case AActivity::Zub: timeZub += duration; break;
        case AActivity::Service: timeService += duration;  break;
        case AActivity::Buero: timeBuero += duration;  break;
        case AActivity::Werkstatt: timeWerkstatt += duration;  break;
        default: break;
        }
        timeSum += duration;
    }
    timeTf = timeTf/(3600000);
    timeZf = timeZf/(3600000);
    timeZub = timeZub/(3600000);
    timeService = timeService/(3600000);
    timeBuero = timeBuero/(3600000);
    timeWerkstatt = timeWerkstatt/(3600000);
    timeSum = timeSum/(3600000);

    sumKilometer = getAnzahl()*strecke;
    valuesInvalid = false;
}

bool Person::addActivity(AActivity *a, AActivity::Category category)
{
    activities->insert(a, category);
    valuesInvalid = true;
    return true;
}

bool Person::removeActivity(AActivity *a)
{
    if (activities->remove(a) != 0) {
        valuesInvalid = true;
        return true;
    }
    return false;
}

QListIterator<AActivity *> *Person::getActivities()
{
    QListIterator<AActivity*> *i = new QListIterator<AActivity*>(activities->keys());
    return i;
}

QString Person::getName() const
{
    return name;
}

void Person::setName(const QString &value)
{
    QString old = name;
    name = value;
    emit nameChanged(this, old);
}

double Person::getTimeTf()
{
    if (valuesInvalid) berechne();
    return timeTf;
}

double Person::getTimeZf()
{
    if (valuesInvalid) berechne();
    return timeZf;
}

double Person::getTimeZub()
{
    if (valuesInvalid) berechne();
    return timeZub;
}

double Person::getTimeService()
{
    if (valuesInvalid) berechne();
    return timeService;
}

double Person::getTimeBuero()
{
    if (valuesInvalid) berechne();
    return timeBuero;
}

double Person::getTimeWerkstatt()
{
    if (valuesInvalid) berechne();
    return timeWerkstatt;
}

double Person::getTimeSum()
{
    if (valuesInvalid) berechne();
    return timeSum;
}

double Person::getSumKilometer()
{
    if (valuesInvalid) berechne();
    return sumKilometer;
}

double Person::getAnzahl()
{
    return activities->size();
}
