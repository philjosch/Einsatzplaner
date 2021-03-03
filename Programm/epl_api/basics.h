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

const QMap<Art, QString> FARBE_FAHRTAGE = {{Museumszug, "#ffffff"},
                                          {Sonderzug, "#ffcccc"},
                                          {Gesellschaftssonderzug, "#FFDA91"},//ffcc66"},
                                          {Nikolauszug, "#ffccff"},
                                          {ELFundMuseumszug, "#e7e7fd"},
                                          {Schnupperkurs, "#918fe3"},
                                          {Bahnhofsfest, "#80e3b1"},
                                          {SonstigerFahrtag, "#ffeb90"},
                                          {Arbeitseinsatz, "#CCBEBE"}
                                         };

enum Mistake {
    // General errors
    OK, // Operation wurde erfolgreich durchgefuehrt
    SonstigerFehler, // Unbestimmter Fehler

    // Fehler bei der Verteilung der Sitzplaetze
    KapazitaetUeberlauf // Es gibt nicht genuegend Sitzplaetze fuer die Reservierungen
};


struct Infos {
    QTime beginn;
    QTime ende;
    Category kategorie;
    QString bemerkung;
    bool anrechnen;
};

QString minutesToHourString(int min);
QString minutesToHourStringShort(int min);

Category getCategoryFromLocalizedString(QString s);
QString getLocalizedStringFromCategory(Category c);

QString getStringFromArt(Art art);

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

    void insertJson(QJsonObject *o) {
        o->insert("startdate", zuString(startdate));
        o->insert("enddate", zuString(enddate));
        o->insert("activities", activities);
    }
    static Auswahl fromJson(QJsonObject o) {
        Auswahl a;
        a.startdate = anfangAusString(o.value("startdate").toString());
        a.enddate = endeAusString(o.value("enddate").toString());
        a.activities = o.value("activities").toBool(true);
        return a;
    }

    static AnfangBedingung anfangAusString(QString s) {
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
    static QString zuString(AnfangBedingung a) {
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

    static EndeBedingung endeAusString(QString s) {
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
    static QString zuString(EndeBedingung e) {
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
};


#endif // BASICS_H
