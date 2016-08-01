#include "fahrtag.h"


Fahrtag::Fahrtag(QDate *date): ManagerReservierungen(), AActivity(date)
{
    connect(this, SIGNAL(activityChanged(AActivity*)), this, SLOT(handleActivity(AActivity*)));
    art = Fahrtag::Museumszug;
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

Fahrtag::Art Fahrtag::getArt() const
{
    return art;
}

void Fahrtag::setArt(const Fahrtag::Art &value)
{
    art = value;
    emit fahrtagModified(this);
}
