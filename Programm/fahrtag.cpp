#include "fahrtag.h"


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
