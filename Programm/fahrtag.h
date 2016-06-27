#ifndef FAHRTAG_H
#define FAHRTAG_H

#include "activity.h"
#include "managerreservierungen.h"

class Fahrtag : public Activity, ManagerReservierungen
{
public:
    Fahrtag();

    enum Art { Museumszug, Sonderzug, Nikolauszug, Schnupperkurs, ELFundMuseumszug, Bahnhofsfest, Sonstiges=100 };

#endif // FAHRTAG_H
