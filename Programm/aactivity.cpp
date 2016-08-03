#include "aactivity.h"
#include <QMap>

AActivity::AActivity(QDate *date, ManagerPersonal *p)
{
    datum = date;
    ort = "";
    zeitAnfang = new QTime(10, 0);
    zeitEnde = new QTime(16, 0);
    anlass = "";
    bemerkungen = "";
    personen = new QMap<Person *, QList<QObject *> *>();
    personalBenoetigt = true;
    personal = p;
}

AActivity::~AActivity()
{
    // Löscht das Objekt und entfernt alle Verbindungen zu den personen
}

void AActivity::fromJson(QJsonObject *o)
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
    handleEmit();
}

QString AActivity::getOrt() const
{
    return ort;
}

void AActivity::setOrt(const QString &value)
{
    ort = value;
}

QTime *AActivity::getZeitAnfang() const
{
    return zeitAnfang;
}

void AActivity::setZeitAnfang(QTime *value)
{
    zeitAnfang = value;
}

QTime *AActivity::getZeitEnde() const
{
    return zeitEnde;
}

void AActivity::setZeitEnde(QTime *value)
{
    zeitEnde = value;
}

QString AActivity::getAnlass() const
{
    return anlass;
}

void AActivity::setAnlass(const QString &value)
{
    anlass = value;
    handleEmit();
}

QString AActivity::getBemerkungen() const
{
    return bemerkungen;
}

void AActivity::setBemerkungen(const QString &value)
{
    bemerkungen = value;
    handleEmit();
}

bool AActivity::getPersonalBenoetigt() const
{
    return personalBenoetigt;
}

void AActivity::setPersonalBenoetigt(bool value)
{
    personalBenoetigt = value;
    handleEmit();
}

QMap<Person *, QList<QObject *> *> *AActivity::getPersonen() const
{
    return personen;
}

bool AActivity::removePerson(Person *p)
{

}

ManagerPersonal::Misstake AActivity::addPerson(Person *p, QString *s, QTime *start, QTime *ende)
{

}

void AActivity::setPersonen(QMap<Person *, QList<QObject *> *> *value)
{
    personen = value;
    handleEmit();
}
/*
QString AActivity::getListString()
{
    return datum->toString("dddd dd.MM.yyyy");
}
*/
/*
QString AActivity::getListStringShort()
{
    if (anlass.length() == 0) {
        return "Aktivität - "+ort;
    }
    return anlass;
}
*/
