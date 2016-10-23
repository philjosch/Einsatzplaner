#ifndef VERTEILER_H
#define VERTEILER_H

#include <QList>
#include <QSet>

#include "reservierung.h"
#include "wagen.h"

class Verteiler
{
public:
    Verteiler(QList<Wagen*> *wagen, QSet<Reservierung*> *reservierungen);
    bool verteile();
    void setCheckAll(bool value);

private:
    void verteile(double bewertung, QSet<Reservierung*> rest, int puffer);
    void dePlatziere(Reservierung *res);
    bool platziere(Reservierung *res);

    double gradeYes(Reservierung *res);
    double gradeNo(Reservierung *res);

    void weisePlaetzeZu();

    double getMinBewertung(QSet<Reservierung *> *liste);

    QList<Wagen*> *wagen;
    QSet<Reservierung*> *reservierungen;
    bool found;
    double besteBewertung;
    Wagen *aktuellerWagen;
    int count;
    bool checkAll;
    double mindestbewertung;
};

#endif // VERTEILER_H
