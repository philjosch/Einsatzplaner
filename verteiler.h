#ifndef VERTEILER_H
#define VERTEILER_H

#include <QSet>
#include <QList>
#include <QHash>
#include <QListIterator>
#include "reservierung.h"
#include "wagen.h"

class Verteiler
{
public:
    Verteiler(QList<Wagen*> *wagen, QList<Reservierung*> *reservierungen);
public slots:
    void verteile();

private:

    void verteile(int bewertung);

    QList<Wagen*> *wagen;
    QList<Reservierung*> *reservierungen;

    bool found;
    double obereGrenze;
    int aktuellerWagen;

    QSet<int> *rest;

    int puffer;



    void verteileSitzplaetze(QList<Wagen *> *wagen, QList<Reservierung *> *res); // Ein Algorithmus, der die Sitzplätze der einzelnen Reservierungen verteilt. Muss erst noch entwickelt werden

    double bewertung;
    double gradeYes(Reservierung *res);
    double gradeNo(Reservierung *res);
    bool platziere(Reservierung *res);
    void dePlatziere(Reservierung *res);
    double minimum_grad;
    int aktWagen;

    int count;

    int minimum(int a, int b);

    QSet<int> *lll;

};

#endif // VERTEILER_H
