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


struct Auswahl {
    enum AnfangBedingung {
        AbHeute,
        AbJetzt,
        AbDatum,
        AbAnfangDesJahres,
        AbAlle
    };
    enum EndeBedingung {
        BisHeute,
        BisJetzt,
        BisDatum,
        BisEndeDesJahres,
        BisAlle,
        BisEndeNaechsterWoche,
        BisEndeNaechsterMonat
    };

    AnfangBedingung startdate = AbJetzt;
    EndeBedingung enddate = BisAlle;
    bool activities = true;

    void insertJson(QJsonObject *o) const;
    static Auswahl fromJson(QJsonObject o);

    static AnfangBedingung anfangAusString(QString s);
    static QString zuString(AnfangBedingung a);

    static EndeBedingung endeAusString(QString s);
    static QString zuString(EndeBedingung e);
};

class AActivity;
class Person;

struct Einsatz {
    Person *person;
    AActivity *activity;
    Category kategorie;
    QTime beginn;
    QTime ende;
    QString bemerkung;
    bool anrechnen;

    static void sort(QList<Einsatz*> *liste);
    static void sort(QList<Einsatz> *liste);

    static bool lesser(Einsatz lhs, Einsatz rhs);
    static bool lesserPoint(const Einsatz *lhs, const Einsatz *rhs);
};

#endif // BASICS_H
