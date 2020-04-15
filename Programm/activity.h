#ifndef ACTIVITY_H
#define ACTIVITY_H

#include "aactivity.h"

class Activity: public AActivity
{
    Q_OBJECT

public:
    Activity(QDate date, ManagerPersonal *p);
    Activity(QJsonObject o, ManagerPersonal *p);
    ~Activity();
};
#endif // ACTIVITY_H
