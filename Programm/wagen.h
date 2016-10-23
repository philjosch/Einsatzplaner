#ifndef WAGEN_H
#define WAGEN_H

#include <QList>

#include "reservierung.h"

class Wagen
{
public:
    Wagen(int nummer);

    int getNummer() const;

    static int klasse(int nummer);
    int klasse();

    int getAnzahlFreiePlaetzeInSitzgruppe();

    void verlassePlaetze(QList<int> *liste);

    QList<int> *besetzePlaetze(Reservierung *r);
    bool besetzePlaetze(Reservierung *r, QList<int> *plaetze);

    double getStrafpunkteFuerPlaetze(int anzahl, int start = 0);

    bool testPlaetze(QList<int> *liste, Reservierung *r); // Testet ob die Plätze frei sind, externe Darstellung

    int getAnzahlFrei();
    int getAnzahlBelegt();
    int getKapazitaet();

    bool isEmpty();

    void weisePlaetzeZu();

    int getAktuellePosition() const;

    void clear();

protected:
    static double FAKTOR_FUER_FREIE_PLAETZE;
    static double GEWICHT_STRAFE_TEIL_1;
    static double GEWICHT_STRAFE_TEIL_2;

private slots:
    QList<int> extToInt(QList<int> *liste);
    QList<int> *intToExt(QList<int> liste);
    bool test(QList<int> liste, Reservierung *r); // Testet mit der Internen Darstellung

private:
    int nummer;

    int aktuellePosition;
    int size;
    int reihe1;
    int reihe2;

    QList<int> *sitzGruppen;
    QHash<int, Reservierung *> *verteilung;

    QList<int> *internToExtern; // Übersetzt die Sitzplätze von der internen Kodierung in die Externe, menschenlesbare Form
    QHash<int, int> *externToIntern; // Reverse von internToExtern, nur als Hash
};

#endif // WAGEN_H
