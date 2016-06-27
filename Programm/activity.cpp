#include "activity.h"

Activity::Activity()
{

}

Activity::~Activity()
{
    // Löscht das Objekt und entfernt alle Verbindungen zu den personen
}

Activity *Activity::fromJson(QJsonObject *o)
{

}

QJsonObject *Activity::toJson()
{

}

QDate *Activity::getDatum() const
{
    return datum;
}

void Activity::setDatum(QDate *value)
{
    datum = value;
}

QString Activity::getOrt() const
{
    return ort;
}

void Activity::setOrt(const QString &value)
{
    ort = value;
}

QTime *Activity::getZeitAnfang() const
{
    return zeitAnfang;
}

void Activity::setZeitAnfang(QTime *value)
{
    zeitAnfang = value;
}

QTime *Activity::getZeitEnde() const
{
    return zeitEnde;
}

void Activity::setZeitEnde(QTime *value)
{
    zeitEnde = value;
}

QString Activity::getAnlass() const
{
    return anlass;
}

void Activity::setAnlass(const QString &value)
{
    anlass = value;
}

QString Activity::getBemerkungen() const
{
    return bemerkungen;
}

void Activity::setBemerkungen(const QString &value)
{
    bemerkungen = value;
}

bool Activity::getPersonalBenoetigt() const
{
    return personalBenoetigt;
}

void Activity::setPersonalBenoetigt(bool value)
{
    personalBenoetigt = value;
}

QMap<Person *, QList<QObject *> *> *Activity::getPersonen() const
{
    return personen;
}

void Activity::setPersonen(QMap<Person *, QList<QObject *> *> *value)
{
    personen = value;
}
