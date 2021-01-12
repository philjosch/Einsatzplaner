#ifndef MANAGER_H
#define MANAGER_H

#include "fahrtag.h"
#include "activity.h"
#include "managerpersonal.h"
#include "filesettings.h"

#include <QJsonObject>

class Manager : public QObject
{
    Q_OBJECT

public:
    Manager();
    QJsonArray toJson();
    void fromJson(QJsonArray array);

    ManagerPersonal *getPersonal() const;

    QList<AActivity *> getActivities();

    static QString getHtmlFuerListenansicht(QList<AActivity *> liste);
    static QString getHtmlFuerEinzelansichten(QList<AActivity *> liste);

signals:
    void changed();

public slots:
    Fahrtag *newFahrtag(QDate datum);
    Activity *newActivity(QDate datum);

    bool removeActivity(AActivity *a);

    void sort();

    QList<AActivity*> filter(AActivity::Auswahl auswahl);

protected:
    void update(int pos);

    QList<AActivity *> activities;
    ManagerPersonal *personal;

};

#endif // MANAGER_H
