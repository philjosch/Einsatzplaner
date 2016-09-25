#ifndef WAGEN_H
#define WAGEN_H

#include "reservierung.h"

#include <QList>



class Wagen
{
public:
    Wagen(int nummer);

    int getNummer() const;

    static int klasse(int nummer);

    int getAnzahlFreiePlaetzeInSitzgruppe();

    void verlassePlaetze(QList<int> *liste);

    QList<int> *besetzePlaetze(Reservierung *r);

    double getStrafpunkteFuerPlaetze(int anzahl, int start = -1);

    int getFreiePlaetze();

    bool isEmpty();

    void print();

    void weisePlaetzeZu();

    int getAktuellePosition() const;

protected:
    static double FAKTOR_FUER_FREIE_PLAETZE;
    static double GEWICHT_STRAFE_TEIL_1;
    static double GEWICHT_STRAFE_TEIL_2;

private:
    int nummer;

    int aktuellePosition;
    int size;
    int reihe1;
    int reihe2;

    QList<int> *sitzGruppen;
    QList<Reservierung *> *verteilung;
};

#endif // WAGEN_H
