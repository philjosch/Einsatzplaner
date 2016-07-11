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
    Activity(QDate *date);
    ~Activity();
    QString getListStringShort();
signals:
    void activityModified(AActivity *a);
    void activityChanged(AActivity *a);
public slots:
    void handleActivity(AActivity *a);
};
#endif // ACTIVITY_H
