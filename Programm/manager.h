#ifndef MANAGER_H
#define MANAGER_H

#include "fahrtag.h"
#include "activity.h"
#include "managerpersonal.h"
#include <QJsonObject>
#include <QList>

class Manager
{
public:
    Manager();
    void fetchPersonal(ManagerPersonal *m);
    QJsonObject toJson();
    void fromJson(QJsonObject *o);

public slots:
    Fahrtag *newFahrtag(QDate *datum);
    Activity *newActivity(QDate *datum);

    bool removeActivity(AActivity *a);
    void addActivity(AActivity *a);

    void activityChanged(AActivity *a);

protected:
    QList<AActivity*> *activities;
    void update(int pos);
};

#endif // MANAGER_H
