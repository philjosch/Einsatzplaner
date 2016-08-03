#include "activity.h"

Activity::Activity(QDate *date, ManagerPersonal *p): AActivity(date, p)
{
}

Activity::~Activity()
{
    AActivity::~AActivity();
}

QString Activity::getListStringShort() {
    if (anlass == "") {
        return "Aktivität";
    }
    return anlass;
}

QString Activity::getListString()
{
    return datum->toString("dddd dd.MM.yyyy")+" – Aktivität";
}

void Activity::handleActivity(AActivity *a)
{
    emit activityModified(a);
}

void Activity::handleEmit()
{
    handleActivity(this);
}
