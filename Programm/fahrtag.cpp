#include "fahrtag.h"
#include <QDebug>
#include <QTime>

Fahrtag::Fahrtag(QDate *date, ManagerPersonal *p): ManagerReservierungen(), AActivity(date, p)
{
    art = Fahrtag::Museumszug;
    wichtig = false;
    zeitTf = new QTime(8, 15);
    zeitAnfang = new QTime(8, 45);
    zeitEnde = new QTime(18, 45);
    benoetigeTf = true;
    benoetigeZf = true;
    benoetigeZub = true;
    benoetigeService = true;

    // Listen für Tf, Zf, Zub und Servie müssen noch initalisiert werden
    listTf = new QMap<QString, QString>();
    listZf = new QMap<QString, QString>();
    listZub = new QMap<QString, QString>();
    listService = new QMap<QString, QString>();
}

QString Fahrtag::getStringFromArt(Fahrtag::Art art)
{
    switch (art) {
    case Museumszug: return "Museumszug";
    case Sonderzug: return "Sonderzug";
    case Nikolauszug: return "Nikolauszug";
    case Schnupperkurs: return "Ehrenlokführer Schnupperkurs";
    case Bahnhofsfest: return "BAhnhofsfest";
    case ELFundMuseumszug: return "Museumszug mit Schnupperkurs";
    default: return "Sonstiges";
    }
}

QString Fahrtag::getListString()
{
    return datum->toString("dddd dd.MM.yyyy")+" – Fahrtag";
}

QString Fahrtag::getListStringShort() {
    if (anlass.length() == 0) {
        return "Fahrtag";
    }
    return anlass;
}

void Fahrtag::handleActivity(AActivity *a) {
    emit fahrtagModified(a);
}

void Fahrtag::handleEmit()
{
    handleActivity(this);
}

QMap<QString, QString> *Fahrtag::getListService() const
{
    return listService;
}

bool Fahrtag::removeService(QString p)
{
    return listService->remove(p) == 1;
}

bool Fahrtag::addService(QString p, QString bemerkungen)
{
    listService->insert(p, bemerkungen);
    QList<int> *l = new QList<int>();
    l->append(0);
    l->append(0);
    l->append((int)Service);
    l->append(4);
    personen->insert(personal->getPerson(p), l);
    return true;
}

QList<int> *Fahrtag::getIndividual(Person *person)
{
    QList<int> *l = new QList<int>();
    QList<int> *orig = personen->value(person);
    if (orig->at(0) == 0) {
        if (orig->at(2) == (int)Tf || orig->at(2) == (int)Tb) {
            l->append(zeitTf->msecsSinceStartOfDay());
        } else {
            l->append(zeitAnfang->msecsSinceStartOfDay());
        }
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

    return personen->value(person);
}

QMap<QString, QString> *Fahrtag::getListZub() const
{
    return listZub;
}

bool Fahrtag::removeZub(QString p)
{
    return listZub->remove(p) == 1;
}

bool Fahrtag::addZub(QString p, QString bemerkungen)
{
    listZub->insert(p, bemerkungen);
    QList<int> *l = new QList<int>();
    l->append(0);
    l->append(0);
    l->append((int)Begleiter);
    l->append(3);
    personen->insert(personal->getPerson(p), l);
    return true;
}

QMap<QString, QString> *Fahrtag::getListZf() const
{
    return listZf;
}

bool Fahrtag::removeZf(QString p)
{
    return listZf->remove(p) == 1;
}

bool Fahrtag::addZf(QString p, QString bemerkungen)
{
    listZf->insert(p, bemerkungen);
    QList<int> *l = new QList<int>();
    l->append(0);
    l->append(0);
    l->append((int)Zf);
    l->append(2);
    personen->insert(personal->getPerson(p), l);
    return true;
}

QMap<QString, QString> *Fahrtag::getListTf() const
{
    return listTf;
}

bool Fahrtag::removeTf(QString p)
{
    return listTf->remove(p) == 1;
}

bool Fahrtag::addTf(QString p, QString bemerkungen)
{
    listTf->insert(p, bemerkungen);
    QList<int> *l = new QList<int>();
    l->append(0);
    l->append(0);
    if (bemerkungen.contains("Tb") || bemerkungen.contains("TB")) {
        l->append((int)Tb);
    } else {
        l->append((int)Tf);
    }
    l->append(1);
    personen->insert(personal->getPerson(p), l);
    return true;

}

bool Fahrtag::getBenoetigeService() const
{
    return benoetigeService;
}

void Fahrtag::setBenoetigeService(bool value)
{
    benoetigeService = value;
    handleEmit();
}

bool Fahrtag::getBenoetigeZub() const
{
    return benoetigeZub;
}

void Fahrtag::setBenoetigeZub(bool value)
{
    benoetigeZub = value;
    handleEmit();
}

bool Fahrtag::getBenoetigeZf() const
{
    return benoetigeZf;
}

void Fahrtag::setBenoetigeZf(bool value)
{
    benoetigeZf = value;
    handleEmit();
}

bool Fahrtag::getBenoetigeTf() const
{
    return benoetigeTf;
}

void Fahrtag::setBenoetigeTf(bool value)
{
    benoetigeTf = value;
    handleEmit();
}

bool Fahrtag::getWichtig() const
{
    return wichtig;
}

void Fahrtag::setWichtig(bool value)
{
    wichtig = value;
    handleEmit();
}

QTime *Fahrtag::getZeitTf() const
{
    return zeitTf;
}

void Fahrtag::setZeitTf(QTime *value)
{
    zeitTf = value;
    handleEmit();
}

Fahrtag::Art Fahrtag::getArt() const
{
    return art;
}

void Fahrtag::setArt(const Fahrtag::Art &value)
{
    art = value;
    handleEmit();
}
