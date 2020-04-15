#include "activity.h"

Activity::Activity(QDate date, ManagerPersonal *p): AActivity(date, p)
{
}

Activity::Activity(QJsonObject o, ManagerPersonal *p) : AActivity(o, p)
{
}

Activity::~Activity()
{
    AActivity::~AActivity();
}
