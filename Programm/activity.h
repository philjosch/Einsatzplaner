#ifndef ACTIVITY_H
#define ACTIVITY_H

#include "person.h"
#include "aactivity.h"

class Activity: public AActivity
{
    Q_OBJECT

public:
    Activity(QDate date, ManagerPersonal *p);
    Activity(QJsonObject o, ManagerPersonal *p);
    ~Activity();

    QJsonObject toJson();

    QString getKurzbeschreibung();
    QString getListStringShort();
    QString getListString();

    Infos getIndividual(Person *person);

    QString getHtmlForSingleView();
    QString getHtmlForTableView();
};
#endif // ACTIVITY_H
