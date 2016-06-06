#ifndef MANAGER_H
#define MANAGER_H

#include "fahrtag.h"
#include "activity.h"

class Manager
{
public:
    Manager();

//public slots:
    Fahrtag *newFahrtag();
    Activity *newActivity();

    bool removeActivity(Activity *a);
    void addActivity(Activity *a);
};

#endif // MANAGER_H
