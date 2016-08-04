#include "person.h"
#include <QMap>

Person::Person(QString name) : QObject()
{
    this->name = name;
    strecke = 0;
    this->ausbildungTf = false;
    this->ausbildungZf = false;
    this->ausbildungRangierer = false;
    timeTf = 0;
    timeZf = 0;
    timeZub = 0;
    timeService = 0;
    timeBuero = 0;
    timeWerkstatt = 0;
    timeSum = 0;
    sumKilometer = 0;
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
    sumKilometer = 0;
    // hier werden die einzelnen Zeiten berechnet und dann in den Variablen gespeichert
    valuesInvalid = false;

}

bool Person::addActivity(AActivity *a, AActivity::Category category)
{
    if (activities->contains(a)) {
        return false;
    }
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

int Person::getTimeTf()
{
    if (valuesInvalid) berechne();
    return timeTf;
}

int Person::getTimeZf()
{
    if (valuesInvalid) berechne();
    return timeZf;
}

int Person::getTimeZub()
{
    if (valuesInvalid) berechne();
    return timeZub;
}

int Person::getTimeService()
{
    if (valuesInvalid) berechne();
    return timeService;
}

int Person::getTimeBuero()
{
    if (valuesInvalid) berechne();
    return timeBuero;
}

int Person::getTimeWerkstatt()
{
    if (valuesInvalid) berechne();
    return timeWerkstatt;
}

int Person::getTimeSum()
{
    if (valuesInvalid) berechne();
    return timeSum;
}

int Person::getSumKilometer()
{
    if (valuesInvalid) berechne();
    return sumKilometer;
}

int Person::getAnzahl()
{
    return activities->size();
}



