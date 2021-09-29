#include "einsatz.h"

#include "aactivity.h"

Einsatz::Einsatz()
{
    person = nullptr;
    activity = nullptr;
    kategorie = Category::Sonstiges;
    beginn = QTime(0,0);
    ende = QTime(0,0);
    bemerkung = "";
}
Einsatz::Einsatz(Person *p, AActivity *a, Category kat, QString bem, QTime von, QTime bis)
{
    person = p;
    activity = a;
    kategorie = kat;
    beginn = von;
    ende = bis;
    bemerkung = bem;
}

void Einsatz::sort(QList<Einsatz *> *liste)
{
    std::sort(liste->begin(), liste->end(), Einsatz::lesserEinsatz);
}

bool operator<(const Einsatz &lhs, const Einsatz &rhs)
{
    if (AActivity::lesser(lhs.activity,rhs.activity))
        return true;
    if (AActivity::lesser(rhs.activity,lhs.activity))
        return false;

    if (lhs.beginn < rhs.beginn)
        return true;
    if (lhs.beginn > rhs.beginn)
        return false;

    if (lhs.ende < rhs.ende)
        return true;
    if (lhs.ende > rhs.ende)
        return false;

    return false;
}

Person *Einsatz::getPerson() const
{
    return person;
}

AActivity *Einsatz::getActivity() const
{
    return activity;
}

Category Einsatz::getKategorie() const
{
    return kategorie;
}

QString Einsatz::getBemerkung() const
{
    return bemerkung;
}
void Einsatz::setBemerkung(const QString &value)
{
    bemerkung = value;
}

QTime Einsatz::getBeginn() const
{
    QTime zeit = activity->getAnfang(kategorie);
    if (zeit.isValid() && beginn != QTime(0,0)) {
        return beginn;
    }
    return zeit;
}
QTime Einsatz::getBeginnAbweichend() const
{
    return beginn;
}
void Einsatz::setBeginnAbweichend(const QTime &value)
{
    beginn = value;
}

QTime Einsatz::getEnde() const
{
    QTime zeit = activity->getEnde(kategorie);
    if (zeit.isValid() && ende != QTime(0,0)) {
        return ende;
    }
    return zeit;
}
QTime Einsatz::getEndeAbweichend() const
{
    return ende;
}
void Einsatz::setEndeAbweichend(const QTime &value)
{
    ende = value;
}

bool Einsatz::getAnrechnen() const
{
    return (activity->getDatum() <= QDate::currentDate() && !activity->getAbgesagt());
}
