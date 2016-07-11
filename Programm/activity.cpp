#include "activity.h"

Activity::Activity(QDate *date): AActivity(date)
{
}

Activity::~Activity()
{
    AActivity::~AActivity();
}

QString Activity::getListStringShort() {
    if (anlass == "") {
        return "Aktivität - "+ort;
    }
    return anlass;
}

void Activity::handleActivity(AActivity *a)
{
    emit activityModified(a);
}
