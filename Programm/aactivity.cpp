#include "aactivity.h"

AActivity::AActivity(QDate *date)
{
    datum = date;
}

AActivity::~AActivity()
{
    // Löscht das Objekt und entfernt alle Verbindungen zu den personen
}

AActivity *AActivity::fromJson(QJsonObject *o)
{

}

QJsonObject *AActivity::toJson()
{

}

QDate *AActivity::getDatum() const
{
    return datum;
}

void AActivity::setDatum(QDate *value)
{
    datum = value;
    emit activityChanged(this);
}

QString AActivity::getOrt() const
{
    return ort;
}

void AActivity::setOrt(const QString &value)
{
    ort = value;
    emit activityChanged(this);
}

QTime *AActivity::getZeitAnfang() const
{
    return zeitAnfang;
}

void AActivity::setZeitAnfang(QTime *value)
{
    zeitAnfang = value;
    emit activityChanged(this);
}

QTime *AActivity::getZeitEnde() const
{
    return zeitEnde;
}

void AActivity::setZeitEnde(QTime *value)
{
    zeitEnde = value;
    emit activityChanged(this);
}

QString AActivity::getAnlass() const
{
    return anlass;
}

void AActivity::setAnlass(const QString &value)
{
    anlass = value;
    emit activityChanged(this);
}

QString AActivity::getBemerkungen() const
{
    return bemerkungen;
}

void AActivity::setBemerkungen(const QString &value)
{
    bemerkungen = value;
    emit activityChanged(this);
}

bool AActivity::getPersonalBenoetigt() const
{
    return personalBenoetigt;
}

void AActivity::setPersonalBenoetigt(bool value)
{
    personalBenoetigt = value;
    emit activityChanged(this);
}

QMap<Person *, QList<QObject *> *> *AActivity::getPersonen() const
{
    return personen;
}

void AActivity::setPersonen(QMap<Person *, QList<QObject *> *> *value)
{
    personen = value;
    emit activityChanged(this);
}

QString AActivity::getListString()
{
    return datum->toString("dddd dd.MM.yyyy");
}

QString AActivity::getListStringShort()
{
    return anlass;
}
