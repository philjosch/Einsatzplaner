#include "basics.h"

#include <QObject>

QString minutesToHourString(int min)
{
    return minutesToHourStringShort(min)+" h";
}

QString minutesToHourStringShort(int min)
{
    return QString("%1:%2").arg(int(min/60)).arg(min % 60, 2, 10,QLatin1Char('0'));
}

Category getCategoryFromLocalizedString(QString s)
{
    s = s.toUpper();
    if (s==QObject::tr("Tf").toUpper()) return Tf;
    if (s==QObject::tr("Tb").toUpper()) return Tb;
    if (s==QObject::tr("Zf").toUpper()) return Zf;
    if (s==QObject::tr("Service").toUpper()) return Service;
    if (s==QObject::tr("Zugbegleiter").toUpper()) return Zub;
    if (s==QObject::tr("Begl.o.b.A.").toUpper()) return Begleiter;
    if (s==QObject::tr("Büro").toUpper()) return Buero;
    if (s==QObject::tr("Werkstatt").toUpper()) return Werkstatt;
    if (s==QObject::tr("Zug Vorbereiten").toUpper()) return ZugVorbereiten;
    if (s==QObject::tr("Ausbildung").toUpper()) return Ausbildung;
    if (s==QObject::tr("Kilometer").toUpper()) return Kilometer;
    if (s==QObject::tr("Gesamt").toUpper()) return Gesamt;
    if (s==QObject::tr("Anzahl").toUpper()) return Anzahl;
    return Sonstiges;
}

QString getLocalizedStringFromCategory(Category c)
{
    switch (c) {
    case Tf: return QObject::tr("Tf");
    case Tb: return QObject::tr("Tb");
    case Zf: return QObject::tr("Zf");
    case Service: return QObject::tr("Service");
    case Zub: return QObject::tr("Zugbegleiter");
    case Begleiter: return QObject::tr("Begl.o.b.A.");
    case Buero: return QObject::tr("Büro");
    case Werkstatt: return QObject::tr("Werkstatt");
    case ZugVorbereiten: return QObject::tr("Zug Vorbereiten");
    case Ausbildung: return QObject::tr("Ausbildung");
    case Kilometer: return QObject::tr("Kilometer");
    case Gesamt: return QObject::tr("Gesamt");
    case Anzahl: return QObject::tr("Anzahl");
    case Sonstiges: return QObject::tr("Sonstiges");
    }
}

QString stringForDurationEditorFromMinutes(int m)
{
    return QString("%1:%2").arg(int(m/60), 4, 10, QLatin1Char('0')).arg(m % 60, 2, 10,QLatin1Char('0'));
}

int minutesFromStringForDurationEditor(QString s)
{
    if (s.contains(":")) {
        QStringList l = s.split(":");
        return l.at(0).toInt()*60+l.at(1).toInt();
    }
    return s.toInt();
}
