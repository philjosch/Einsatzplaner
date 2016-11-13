#ifndef MANAGER_H
#define MANAGER_H

#include <QJsonObject>
#include <QList>

#include "fahrtag.h"
#include "activity.h"
#include "managerpersonal.h"

class Manager
{
public:
    Manager();
    QJsonObject toJson();
    QJsonObject personalToJson();
    void fromJson(QJsonObject o);

    void setPersonal(ManagerPersonal *value);
    ManagerPersonal *getPersonal() const;

    QListIterator<AActivity *> getActivities() const;

public slots:
    Fahrtag *newFahrtag(QDate datum);
    Activity *newActivity(QDate datum);

    bool removeActivity(AActivity *a);
    void addActivity(AActivity *a);

    void activityChanged(AActivity *a);

protected:
    QList<AActivity *> *activities;
    void update(int pos);
    ManagerPersonal *personal;

};

#endif // MANAGER_H
