#include "fahrtag.h"


Fahrtag::Fahrtag(QDate *date): ManagerReservierungen(), AActivity(date)
{
    connect(this, SIGNAL(activityChanged(AActivity*)), this, SLOT(handleActivity(AActivity*)));
}

QString Fahrtag::getListString()
{
    return datum->toString("dddd dd.MM.yyyy");
}


QString Fahrtag::getListStringShort() {
    if (anlass.length() == 0) {
        return "Aktivität - "+ort;
    }
    return anlass;
}


void Fahrtag::handleActivity(AActivity *a) {
    emit fahrtagModified(a);
}
