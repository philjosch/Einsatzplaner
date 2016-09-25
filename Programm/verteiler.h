#ifndef VERTEILER_H
#define VERTEILER_H

#include <QList>
#include "reservierung.h"
#include <QSet>
#include "wagen.h"

class Verteiler
{
public:
    Verteiler(QList<Wagen*> *wagen, QSet<Reservierung*> *reservierungen);

    bool verteile();

    void verteile(double bewertung, QSet<Reservierung*> rest, int puffer);

    QList<Wagen*> *wagen;
    QSet<Reservierung*> *reservierungen;

    bool found;
    double besteBewertung;
    Wagen *aktuellerWagen;

    int count;

    void dePlatziere(Reservierung *res);
    bool platziere(Reservierung *res);

    double gradeYes(Reservierung *res);
    double gradeNo(Reservierung *res);

    void weisePlaetzeZu();

    bool checkAll;

    void setCheckAll(bool value);

    double mindestbewertung;

    double getMinBewertung(QSet<Reservierung *> *liste);
};

#endif // VERTEILER_H
