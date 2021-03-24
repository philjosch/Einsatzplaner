#include "auswahl.h"

#include <QJsonObject>

Auswahl::Auswahl()
{
    startdate = AbJetzt;
    enddate = BisAlle;
    activities = true;
    abDatum = QDate();
    bisDatum = QDate();
}

Auswahl::Auswahl(Auswahl::AnfangBedingung ab, Auswahl::EndeBedingung bis, bool arbeitseinsaetze)
{
    startdate = ab;
    enddate = bis;
    activities = arbeitseinsaetze;
    abDatum = QDate();
    bisDatum = QDate();
}

Auswahl::Auswahl(Auswahl::AnfangBedingung ab, QDate abD, Auswahl::EndeBedingung bis, QDate bisD)
{
    startdate = ab;
    abDatum = abD;
    enddate = bis;
    bisDatum = bisD;
    activities = true;
}

Auswahl::Auswahl(QJsonObject o)
{
    startdate = anfangAusString(o.value("startdate").toString());
    enddate = endeAusString(o.value("enddate").toString());
    activities = o.value("activities").toBool(true);
}

QDateTime Auswahl::getBis() const
{
    QDate ref = QDate::currentDate();
    switch (enddate) {
    case BisHeute:
        return ref.endOfDay();
    case BisJetzt:
        return QDateTime::currentDateTime();
    case BisDatum:
        return bisDatum.endOfDay();
    case BisEndeDesJahres:
        return QDate(ref.year(), 12, 31).endOfDay();
    case BisAlle:
        return QDate(9999, 31, 12).endOfDay();
    case BisEndeNaechsterWoche:
        ref = QDate::currentDate().addDays(7); // naechste Woche
        return ref.addDays(7-ref.dayOfWeek()).endOfDay(); // Ende der Woche
    case BisEndeNaechsterMonat:
        ref = QDate::currentDate().addMonths(1); // naechster Monat
        return QDate(ref.year(), ref.month(), ref.daysInMonth()).endOfDay(); // Ende des Monats
    }
}

QDateTime Auswahl::getAb() const
{
    switch (startdate) {
    case AbHeute:
        return QDate::currentDate().startOfDay();
    case AbJetzt:
        return QDateTime::currentDateTime();
    case AbDatum:
        return abDatum.startOfDay();
    case AbAlle:
        return QDate(1900, 1, 1).startOfDay();
    case AbAnfangDesJahres:
        QDate ref = QDate::currentDate();
        return QDate(ref.year(), 12, 31).startOfDay();
    }
}

void Auswahl::insertJson(QJsonObject *o) const
{
    o->insert("startdate", zuString(startdate));
    o->insert("enddate", zuString(enddate));
    o->insert("activities", activities);
}

Auswahl::AnfangBedingung Auswahl::anfangAusString(QString s)
{
    if (s == "tdy") {
        return AbJetzt;
    }
    if (s == "all") {
        return AbAlle;
    }
    if (s == "bgn") {
        return AbAnfangDesJahres;
    }
    return AbAlle;
}
QString Auswahl::zuString(Auswahl::AnfangBedingung a)
{
    switch (a) {
    case AbJetzt:
        return "tdy";
    case AbAlle:
        return "all";
    case AbAnfangDesJahres:
        return "bgn";
    default:
        return "all";
    }
}

Auswahl::EndeBedingung Auswahl::endeAusString(QString s)
{
    if (s == "p1w") {
        return BisEndeNaechsterWoche;
    }
    if (s == "p1m") {
        return BisEndeNaechsterMonat;
    }
    if (s == "eoy") {
        return BisEndeDesJahres;
    }
    if (s == "all") {
        return BisAlle;
    }
    return BisAlle;
}
QString Auswahl::zuString(Auswahl::EndeBedingung e)
{
    switch (e) {
    case BisEndeDesJahres:
        return "eoy";
    case BisAlle:
        return "all";
    case BisEndeNaechsterWoche:
        return "p1w";
    case BisEndeNaechsterMonat:
        return "p1m";
    default:
        return "all";
    }
}

Auswahl::AnfangBedingung Auswahl::getStartdate() const
{
    return startdate;
}
void Auswahl::setStartdate(const AnfangBedingung &value)
{
    startdate = value;
}

Auswahl::EndeBedingung Auswahl::getEnddate() const
{
    return enddate;
}
void Auswahl::setEnddate(const EndeBedingung &value)
{
    enddate = value;
}

bool Auswahl::getActivities() const
{
    return activities;
}
void Auswahl::setActivities(bool value)
{
    activities = value;
}
