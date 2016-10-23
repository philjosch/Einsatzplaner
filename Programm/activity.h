#ifndef ACTIVITY_H
#define ACTIVITY_H

#include <QDate>
#include <QTime>
#include <QObject>

#include "person.h"
#include "aactivity.h"

class Activity: public QObject, public AActivity
{
    Q_OBJECT

public:
    Activity(QDate date, ManagerPersonal *p);
    Activity(QJsonObject o, ManagerPersonal *p);
    ~Activity();

    QJsonObject toJson();

    QString getListStringShort();
    QString getListString();

    AActivity::Infos *getIndividual(Person *person);

    QString getHtmlForSingleView();
    QString getHtmlForTableView();

    void emitter();

signals:
    void changed(AActivity *);
};
#endif // ACTIVITY_H
