#ifndef MANAGER_H
#define MANAGER_H

#include "fahrtag.h"
#include "activity.h"
#include "managerpersonal.h"

#include <QJsonObject>

class Manager
{
public:
    Manager();
    QJsonObject toJson();
    QJsonObject personalToJson();
    void fromJson(QJsonObject c, QJsonObject p);

    void setPersonal(ManagerPersonal *value);
    ManagerPersonal *getPersonal() const;

    QListIterator<AActivity *> getActivities() const;

    static QString getHtmlFuerListenansicht(QList<AActivity *> liste);
    static QString getHtmlFuerEinzelansichten(QList<AActivity *> liste);

public slots:
    Fahrtag *newFahrtag(QDate datum);
    Activity *newActivity(QDate datum);

    bool removeActivity(AActivity *a);
    void addActivity(AActivity *a);

    void activityChanged(AActivity *a);

protected:
    QList<AActivity *> activities;
    void update(int pos);
    ManagerPersonal *personal;

};

#endif // MANAGER_H
