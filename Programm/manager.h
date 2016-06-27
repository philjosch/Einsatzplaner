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
    Fahrtag *newFahrtag();
    Activity *newActivity();

    bool removeActivity(Activity *a);
    void addActivity(Activity *a);

    void activityChanged(Activity *a);

private:
    QList<Activity*> *activities;
    void update(int pos);
};

#endif // MANAGER_H
