#ifndef BASICS_H
#define BASICS_H

#include <QDate>
#include <QJsonObject>
#include <QMap>

enum Category {
    Tf,
    Tb,
    Zf,
    Service,
    Zub,
    Begleiter,
    Buero,
    Werkstatt,
    ZugVorbereiten,
    Ausbildung, // In Version 1.3 eingefuehrt
    Infrastruktur, // In Version 1.6 eingefuehrt
    Sonstiges=100,
    Gesamt=120,   // Wird nur verwendet, um Funktionsaufrufe einfacher zu gestalten
    Kilometer=121,// Wird nur verwendet, um Funktionsaufrufe einfacher zu gestalten
    Anzahl=122    // Wird nur verwendet, um Funktionsaufrufe einfacher zu gestalten
};

Category getCategoryFromLocalizedString(QString s);
QString getLocalizedStringFromCategory(Category c);


const QList<Category> ANZEIGEREIHENFOLGE =                       {Tf, Zf, Zub, Service, ZugVorbereiten, Werkstatt, Buero, Ausbildung, Infrastruktur, Sonstiges};
const QList<Category> ANZEIGEREIHENFOLGEGESAMT = {Gesamt, Anzahl, Tf, Zf, Zub, Service, ZugVorbereiten, Werkstatt, Buero, Ausbildung, Infrastruktur, Sonstiges, Kilometer};


enum Art {
    Museumszug,
    Sonderzug,
    Gesellschaftssonderzug,
    Nikolauszug,
    ELFundMuseumszug,
    Schnupperkurs,
    Bahnhofsfest,
    SonstigerFahrtag,
    Arbeitseinsatz = 100
};

QString getStringFromArt(Art art);


enum Mitglied {
    AlleMitglieder,
    Aktiv,
    AktivMit,
    AktivOhne,
    Passiv,
    PassivMit,
    PassivOhne,
    Ausgetreten,
    Registriert
};

QString getStringVonFilter(Mitglied filter);


QString minutesToHourString(int min);
QString minutesToHourStringShort(int min);

#endif // BASICS_H
