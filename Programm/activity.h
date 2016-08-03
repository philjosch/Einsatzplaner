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

    enum Category { Tf, Tb, Zf, Service, Begleiter, Buero, Werkstatt, ZugVorbereiten, Sonstiges=100 };

    Activity(QDate *date, ManagerPersonal *p);
    ~Activity();
    QString getListStringShort();
    QString getListString();
signals:
    void activityModified(AActivity *a);
private slots:
    void handleActivity(AActivity *a);
    void handleEmit();

};
#endif // ACTIVITY_H
