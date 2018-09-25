#ifndef VERTEILER_H
#define VERTEILER_H

#include "reservierung.h"
#include "wagen.h"
#include "enums.h"

class Verteiler
{
public:
    Verteiler(QList<Wagen*> wagen, QSet<Reservierung*> reservierungen);
    Mistake verteile();
    void setCheckAll(bool value);

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

    void dePlatziere(Reservierung *res);
    bool platziere(Reservierung *res);

    double gradeYes(Reservierung *res);
    double gradeNo(Reservierung *res);

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
    double mindestbewertung;
};

#endif // VERTEILER_H
