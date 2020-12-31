#ifndef BASICS_H
#define BASICS_H

#include <QDate>
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

    // Fehler bei der Personalverwaltung
    ExternOk, // Die Person ist nicht im System bekannt, wurde aber als externe Person akzeptiert
    PassivOk, // Die Person wurde eingetragen, ist allerdings als passives Mitglied gefuehrt

    PersonNichtGefunden, // Die Person wurde nicht gefunden
    FalscheQualifikation, // Die Person hat nicht die noetigen Qualifikationen

    // Fehler bei der Verteilung der Sitzplaetze
    KapazitaetUeberlauf // Es gibt nicht genuegend Sitzplaetze fuer die Reservierungen
};


struct Infos {
    QTime beginn;
    QTime ende;
    Category kategorie;
    QString bemerkung;
};

QString minutesToHourString(int min);
QString minutesToHourStringShort(int min);

Category getCategoryFromLocalizedString(QString s);
QString getLocalizedStringFromCategory(Category c);

QString getStringFromArt(Art art);

struct Version {
    int major;
    int minor;
    int patch;

    QString toString() {
        return QString("%1.%2.%3").arg(major).arg(minor).arg(patch);
    }
    QString toStringShort() {
        return QString("%1.%2").arg(major).arg(minor);
    }
    static Version stringToVersion (QString s) {
        if (! s.contains(".")) return Version {-1, -1, -1};
        QStringList vers = s.split(".");
        if (vers.length() < 2) return Version {-1, -1, -1};
        Version v;
        v.major = vers.at(0).toInt();
        v.minor = vers.at(1).toInt();
        if (vers.length() >= 3) // Dritte Stelle nur nehmen, wenn sie vorhanden ist
            v.patch = vers.at(2).toInt();
        else
            v.patch = -1;
        return v;
    }

    bool operator ==(Version second) {
        if ((major == second.major) && (minor == second.minor) && (patch == second.patch)) {
            return true;
        }
        return false;
    }
    bool operator >(Version second) {
        if ((major > second.major) || (second.major == -1)) {
            return true;
        }
        if ((major == second.major) && ((minor > second.minor) || (second.minor == -1))) {
            return true;
        }
        if ((major == second.major) && (minor == second.minor) && ((patch > second.patch) || (patch != -1 && second.patch == -1))) {
            return true;
        }
        return false;
    }
    bool operator <(Version second) {
        return second>*this;
    }
    bool operator >=(Version second) {
        return *this == second || *this > second;
    }
    bool operator <=(Version second) {
        return *this == second || second > *this;
    }
};

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

#endif // BASICS_H
