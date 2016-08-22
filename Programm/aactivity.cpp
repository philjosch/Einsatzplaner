#include "aactivity.h"
#include <QMap>
#include "person.h"
#include <QDebug>

AActivity::Category AActivity::getCategoryFromString(QString s)
{
    s = s.toUpper();
    if (s=="TF") return Tf;
    if (s=="TB") return Tb;
    if (s=="ZF") return Zf;
    if (s=="SERVICE") return Service;
    if (s=="BEGLEITER" || s=="ZUB") return Begleiter;
    if (s=="BÜRO" || s=="BUERO") return Buero;
    if (s=="WERKSTATT") return Werkstatt;
    if (s=="VORBEREITEN" || s=="ZUG VORBEREITEN" || s=="ZUGVORBEREITEN") return ZugVorbereiten;
    return Sonstiges;
}

QString AActivity::getStringFromCategory(AActivity::Category c)
{
    switch (c) {
    case Tf: return "Tf";
    case Tb: return "Tb";
    case Zf: return "Zf";
    case Service: return "Service";
    case Begleiter: return "Begleiter";
    case Buero: return "Büro";
    case Werkstatt: return "Werkstatt";
    case ZugVorbereiten: return "Zug Vorbereiten";
    default: return "Sonstiges";
    }
}

AActivity::AActivity(QDate *date, ManagerPersonal *p)
{
    datum = date;
    ort = "";
    zeitAnfang = new QTime(10, 0);
    zeitEnde = new QTime(16, 0);
    anlass = "";
    bemerkungen = "";
    personen = new QMap<Person *, QList<int> *>();
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

QMap<Person *, QList<int> *> *AActivity::getPersonen() const
{
    return personen;
}

bool AActivity::removePerson(Person *p)
{

    return p->removeActivity(this);
}

bool AActivity::removePerson(QString p)
{
    Person *pers = personal->getPerson(p);
    if (pers != nullptr) {
        return removePerson(pers);
    }
    return false;
}

ManagerPersonal::Misstake AActivity::addPerson(Person *p, QString s, QTime *start, QTime *ende)
{
   /*
    * angabe über aufgabe prüfen
    * p. ob person geeignet
    * person hinzufügen
    */
    AActivity::Category kat = AActivity::getCategoryFromString(s);
    switch (kat) {
    case Tf:
        if (! p->getAusbildungTf()) {
            return ManagerPersonal::FalscheQualifikation;
        }
        break;
    case Tb:
        // Ein Tb muss auch Lokführer sein, diese muss noch abgeklärt werden: Issue #9
        if (! p->getAusbildungTf()) {
            return ManagerPersonal::FalscheQualifikation;
        }
        break;
    case Zf:
        if (! p->getAusbildungZf()) {
            return ManagerPersonal::FalscheQualifikation;
        }
        break;
    default:
        break;
    }

    // jetzt ist alles richtig und die person kann registiert werden.
    p->addActivity(this, kat);

    QList<int> *liste = new QList<int>();
    liste->append(start->msecsSinceStartOfDay());
    liste->append(ende->msecsSinceStartOfDay());
    liste->append((int)kat);
    personen->insert(p, liste);

    return ManagerPersonal::OK;
}

ManagerPersonal::Misstake AActivity::addPerson(QString p, QString s, QTime *start, QTime *ende)
{
    Person *pers = personal->getPerson(p);
    if (pers != nullptr) {
        return addPerson(pers, s, start, ende);
    }
    return ManagerPersonal::PersonNichtGefunden;
}

void AActivity::setPersonen(QMap<Person *, QList<int> *> *value)
{
    personen = value;
    handleEmit();
}

ManagerPersonal *AActivity::getPersonal() const
{
    return personal;
}
