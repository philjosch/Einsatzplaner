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

const QMap<Art, QString> FARBE_FAHRTAGE = {{Museumszug, "#ffffff"},
                                          {Sonderzug, "#ffcccc"},
                                          {Gesellschaftssonderzug, "#ffcc66"},
                                          {Nikolauszug, "#ffccff"},
                                          {ELFundMuseumszug, "#e7e7fd"},
                                          {Schnupperkurs, "#918fe3"},
                                          {Bahnhofsfest, "#80e3b1"},
                                          {SonstigerFahrtag, "#ffeb90"},
                                          {Arbeitseinsatz, "#CCBEBE"}
                                         };

enum Mistake {
    // General errors
    OK, // Everything went right and the process was successfully done
    SonstigerFehler, // Some other misstake during the process

    // Errors regarding the management of persons
    PersonNichtGefunden, // If the person has not been found and there is no reason to interpret it as an external
    FalscheQualifikation, // registerd person was found but is not qualified to do the job
    ExternOk, // The person was not found in the system, but there is a reason to interpret it as an external

    // Errors regarding the distribution of seats
    KapazitaetUeberlauf // Thrown if there are not enough seats for the reservations
};


struct Infos {
    QTime beginn;
    QTime ende;
    Category kategorie;
    QString bemerkung;
};

QString minutesToHourString(int min);
QString minutesToHourStringShort(int min);

QString stringForDurationEditorFromMinutes(int m);
int minutesFromStringForDurationEditor(QString s);

Category getCategoryFromLocalizedString(QString s);
QString getLocalizedStringFromCategory(Category c);

QString getStringFromArt(Art art);

#endif // BASICS_H
