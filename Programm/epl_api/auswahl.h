#ifndef AUSWAHL_H
#define AUSWAHL_H

#include <QDate>
#include <QObject>

class Auswahl
{
public:
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

    Auswahl();
    Auswahl(AnfangBedingung ab, EndeBedingung bis, bool arbeitseinsaetze);
    Auswahl(AnfangBedingung ab, QDate abD, EndeBedingung bis, QDate bisD);
    Auswahl(QJsonObject o);

    void insertJson(QJsonObject *o) const;

    QDateTime getBis() const;
    QDateTime getAb() const;

    AnfangBedingung getStartdate() const;
    void setStartdate(const AnfangBedingung &value);

    EndeBedingung getEnddate() const;
    void setEnddate(const EndeBedingung &value);

    bool getActivities() const;
    void setActivities(bool value);

protected:
    AnfangBedingung startdate;
    QDate abDatum;
    EndeBedingung enddate;
    QDate bisDatum;

    bool activities;

    static AnfangBedingung anfangAusString(QString s);
    static QString toString(AnfangBedingung a);

    static EndeBedingung endeAusString(QString s);
    static QString toString(EndeBedingung e);
};

#endif // AUSWAHL_H
