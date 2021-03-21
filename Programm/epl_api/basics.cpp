#include "basics.h"

#include "aactivity.h"

#include <QObject>

QString minutesToHourString(int min)
{
    return minutesToHourStringShort(min)+" h";
}

QString minutesToHourStringShort(int min)
{
    if (min <= 0) return "0";
    if (min % 60 == 0)
        return QString("%1").arg(int(min/60));
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
    if (s==QObject::tr("Infrastruktur").toUpper()) return Infrastruktur;
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
    case Infrastruktur: return QObject::tr("Infrastruktur");
    case Kilometer: return QObject::tr("Kilometer");
    case Gesamt: return QObject::tr("Gesamt");
    case Anzahl: return QObject::tr("Anzahl");
    case Sonstiges: return QObject::tr("Sonstiges");
    }
}

QString getStringFromArt(Art art)
{
    switch (art) {
    case Museumszug: return QObject::tr("Museumszug");
    case Sonderzug: return QObject::tr("Sonderzug");
    case Gesellschaftssonderzug: return QObject::tr("Gesellschaftssonderzug");
    case Nikolauszug: return QObject::tr("Nikolauszug");
    case ELFundMuseumszug: return QObject::tr("Museumszug mit Schnupperkurs");
    case Schnupperkurs: return QObject::tr("Ehrenlokführer Schnupperkurs");
    case Bahnhofsfest: return QObject::tr("Bahnhofsfest");
    case SonstigerFahrtag: return QObject::tr("Sonstiges");
    case Arbeitseinsatz: return QObject::tr("Arbeitseinsatz");
    }
}

QString getStringVonFilter(Mitglied filter)
{
    switch (filter) {
    case AlleMitglieder: return QObject::tr("Alle Mitglieder");
    case Aktiv: return QObject::tr("Aktive Mitglieder");
    case AktivMit: return QObject::tr("Aktive Mitglieder mit erbrachten Stunden");
    case AktivOhne: return QObject::tr("Aktive Mitglieder mit fehlenden Stunden");
    case Passiv: return QObject::tr("Passive Mitglieder");
    case PassivMit: return QObject::tr("Passive Mitglieder mit Stunden");
    case PassivOhne: return QObject::tr("Passive Mitglieder ohne Stunden");
    case Ausgetreten: return QObject::tr("Ausgetretene Mitglieder");
    case Registriert: return QObject::tr("Registrierte Personen");
    }
}


void Auswahl::insertJson(QJsonObject *o) const {
    o->insert("startdate", zuString(startdate));
    o->insert("enddate", zuString(enddate));
    o->insert("activities", activities);
}

Auswahl Auswahl::fromJson(QJsonObject o) {
    Auswahl a;
    a.startdate = anfangAusString(o.value("startdate").toString());
    a.enddate = endeAusString(o.value("enddate").toString());
    a.activities = o.value("activities").toBool(true);
    return a;
}

Auswahl::AnfangBedingung Auswahl::anfangAusString(QString s) {
    if (s == "tdy") {
        return AbJetzt;
    }
    if (s == "all") {
        return AbAlle;
    }
    if (s == "bgn") {
        return AbAnfangDesJahres;
    }
    return AbAlle;
}

QString Auswahl::zuString(Auswahl::AnfangBedingung a) {
    switch (a) {
    case AbJetzt:
        return "tdy";
    case AbAlle:
        return "all";
    case AbAnfangDesJahres:
        return "bgn";
    default:
        return "all";
    }
}

Auswahl::EndeBedingung Auswahl::endeAusString(QString s) {
    if (s == "p1w") {
        return BisEndeNaechsterWoche;
    }
    if (s == "p1m") {
        return BisEndeNaechsterMonat;
    }
    if (s == "eoy") {
        return BisEndeDesJahres;
    }
    if (s == "all") {
        return BisAlle;
    }
    return BisAlle;
}

QString Auswahl::zuString(Auswahl::EndeBedingung e) {
    switch (e) {
    case BisEndeDesJahres:
        return "eoy";
    case BisAlle:
        return "all";
    case BisEndeNaechsterWoche:
        return "p1w";
    case BisEndeNaechsterMonat:
        return "p1m";
    default:
        return "all";
    }
}
