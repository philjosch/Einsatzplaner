#ifndef FAHRTAG_H
#define FAHRTAG_H

#include "activity.h"
#include "managerreservierungen.h"

class Fahrtag : public Activity, ManagerReservierungen
{
public:
    Fahrtag();
};

#endif // FAHRTAG_H
