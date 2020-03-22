#include "basics.h"

QString minutesToHourString(int min)
{
    return minutesToHourStringShort(min)+" h";
}

QString minutesToHourStringShort(int min)
{
    return QString("%1:%2").arg(int(min/60)).arg(min % 60, 2, 10,QLatin1Char('0'));
}
