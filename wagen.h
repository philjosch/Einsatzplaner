
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

    int getNummer() const;

public slots:
    void verlasse(QList<int> *liste);
    QList<int> *besetze(Reservierung *r);

    int getStrafpunkte(QList<int> *liste);
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

};

#endif // WAGEN_H
