
#ifndef WAGEN_H
#define WAGEN_H

#include <QVector>
#include <QList>

class Wagen;

#include "reservierung.h"



class Wagen
{
public:
    Wagen(int nummer);
    Wagen();

    int getNummer() const;

    QList<int> *getExtra() const;

    static int klasse(int nummer);

    int getAnzahlBisEnde();

public slots:
    void verlasse(QList<int> *liste);
    QList<int> *besetze(Reservierung *r);

    double getStrafpunkte(QList<int> *liste);
    int getFreiePlaetze();
    bool isEmpty();


private:
    QVector<int> sitzGruppen;
    QVector<Reservierung*> verteilung;
    QVector<int> bewertung;  // Seite [i] = 0 == i liegt aus Niko seite
    int aktuellePosition;
    int size;

    int nummer;
    int row1;
    int row2;

    QList<int> *extra;


};

#endif // WAGEN_H
