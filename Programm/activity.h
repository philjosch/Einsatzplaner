#ifndef ACTIVITY_H
#define ACTIVITY_H

#include <QDate>
#include <QTime>
#include "person.h"
#include <QObject>
#include "aactivity.h"

class Activity: public QObject, public AActivity
{
    Q_OBJECT

public:

    Activity(QDate *date, ManagerPersonal *p);
    ~Activity();
    QString getListStringShort();
    QString getListString();

    AActivity::Infos *getIndividual(Person *person);

signals:
    void activityModified(AActivity *a);
private slots:
    void handleActivity(AActivity *a);
    void handleEmit();

};
#endif // ACTIVITY_H
