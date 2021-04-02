#ifndef MANAGER_H
#define MANAGER_H

#include "fahrtag.h"
#include "managerpersonal.h"
#include "filesettings.h"

#include <QJsonObject>

class Manager : public QObject
{
    Q_OBJECT

public:
    Manager(ManagerPersonal *manPersonal);
    Manager(ManagerPersonal *manPersonal, QJsonArray array);
    QJsonArray toJson() const;

    QList<AActivity *> getActivities() const;

    static QString getHtmlFuerListenansicht(QList<AActivity *> liste);
    static QString getHtmlFuerEinzelansichten(QList<AActivity *> liste);

    Fahrtag *newFahrtag(QDate datum);
    AActivity *newActivity(QDate datum);

    bool removeActivity(AActivity *a);

    QList<AActivity*> filter(Auswahl auswahl) const;

signals:
    void changed();
    void veraenderteAktivitaet(AActivity*, QDate);

protected:
    QList<AActivity *> activities;
    ManagerPersonal *personal;

};

#endif // MANAGER_H
