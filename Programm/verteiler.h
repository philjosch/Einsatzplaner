#ifndef VERTEILER_H
#define VERTEILER_H

#include "reservierung.h"
#include "wagen.h"
#include "basics.h"

class Verteiler
{
public:
    Verteiler(QList<Wagen*> wagen, QSet<Reservierung*> reservierungen, bool all = false);
    Mistake verteile();

    struct Configuration {
        double bewertung;
        QSet<Reservierung *> rest;
        int puffer;
        int aktWagen;
        Reservierung *res;
        QList<int> sitze;
    };

private:

    struct ResBewTuple {
        QList<Reservierung*> res;
        QList<double> bew;
        Reservierung *extra;
    };



    void weisePlaetzeZu();

    double getMinBewertung(QSet<Reservierung *> liste);

    ResBewTuple getNaechsteReservierungen(QSet<Reservierung*> res, int aktWagen, double bewertung, int puffer=0);

    QList<Wagen*> wagen;
    QSet<Reservierung*> reservierungen;
    bool found;
    double besteBewertung;
    int aktWg;
    int count;
    bool checkAll;
};

#endif // VERTEILER_H
