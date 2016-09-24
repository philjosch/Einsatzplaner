#include "verteiler.h"
#include <QDebug>

Verteiler::Verteiler(QList<Wagen*> *wagen, QSet<Reservierung*> *reservierungen)
{
    this->wagen = wagen;
    this->reservierungen = reservierungen;
    found = false;
    besteBewertung = 1215752192;
    aktuellerWagen = wagen->first();
    count = 0;
    checkAll = false;
}

bool Verteiler::verteile()
{
    // berechne untere Schranke für die Güte dees ergebnisses
    mindestbewertung = 0;
    for(Reservierung *r: reservierungen->values()) {
        int min = 1215752192;
        for (Wagen *w: *wagen) {
            for(int start = 0; start < w->getFreiePlaetze(); ++start) {
                int bew = w->getStrafpunkteFuerPlaetze(r->getAnzahl(), start);
                if ((bew < min) && (bew != -1))
                    min = bew;
            }
        }
        mindestbewertung += min;
    }

    qDebug() << "Mindestbewertung: " << mindestbewertung;

    int puffer = 0;
    for(Wagen *w: *wagen) {
        puffer += w->getFreiePlaetze();
    }
    for(Reservierung *r: *reservierungen) {
        puffer -= r->getAnzahl();
    }
    if (puffer < 0) return false;
    verteile(0, *reservierungen, puffer);
    qDebug() << count << "Lösungen";
    return found;
}


void Verteiler::verteile(double bewertung, QSet<Reservierung *> rest, int puffer)
{
    if (bewertung > besteBewertung) {
        return;
    }

    if (rest.isEmpty()) {
        qDebug() << "Lösung von " << besteBewertung << " auf " << bewertung << " verbessert!";
        count ++;
        weisePlaetzeZu();
        besteBewertung = bewertung;
        if (bewertung == mindestbewertung) {
            checkAll = false;
        }
        found = true;
        return;
    }
    if (aktuellerWagen == nullptr) return;

    QHash<int, Reservierung*> anzahlToReservierung;
    QHash<double, QList<int>*> bewertungDerReservierungen;
    for(Reservierung *r: rest.values()) {

        if (anzahlToReservierung.contains(r->getAnzahl())) continue;
        if (aktuellerWagen->getFreiePlaetze() < r->getAnzahl()) continue;

        double bewertungReservierung = aktuellerWagen->getStrafpunkteFuerPlaetze(r->getAnzahl());

        if (bewertung + bewertungReservierung < besteBewertung) {
            anzahlToReservierung.insert(r->getAnzahl(), r);
            if (! bewertungDerReservierungen.contains(bewertungReservierung)) {
                bewertungDerReservierungen.insert(bewertungReservierung, new QList<int>());
            }
            bewertungDerReservierungen.value(bewertungReservierung)->append(r->getAnzahl());
        } else {
            continue;
        }
    }

    int freiePlaetze = aktuellerWagen->getAnzahlFreiePlaetzeInSitzgruppe();
    Reservierung *extra = new Reservierung();
    extra->setAnzahl(freiePlaetze);
    if ((! anzahlToReservierung.contains(freiePlaetze)) && freiePlaetze <= puffer && freiePlaetze > 0) {
        anzahlToReservierung.insert(freiePlaetze, extra);
        double bewertungExtra = aktuellerWagen->getStrafpunkteFuerPlaetze(freiePlaetze);
        if (! bewertungDerReservierungen.contains(bewertungExtra))
            bewertungDerReservierungen.insert(bewertungExtra, new QList<int>());
        bewertungDerReservierungen.value(bewertungExtra)->append(freiePlaetze);
    }

    if(anzahlToReservierung.isEmpty()) return;

    // Sortiere die Liste mit Reservierungen zuerst nach Bewertung und 2. rangig nach Anzahl absteigend
    QList<double> keys = bewertungDerReservierungen.keys();
    qSort(keys);
    QList<int> sortiert;
    QHash<int,double> anzahlToBewertung;
    for (double d: keys) {
        QList<int> *liste = bewertungDerReservierungen.value(d);
        qSort(*liste);
        for(int i = liste->length()-1; i >= 0; --i) {
            sortiert.append(liste->at(i));
            anzahlToBewertung.insert(liste->at(i), d);
        }
        delete liste;
    }
    bewertungDerReservierungen = QHash<double,QList<int>*>();

    for(int j = 0; j < sortiert.length() && (checkAll || ! found); j++) {
        Reservierung *r = anzahlToReservierung.value(sortiert.at(j));
        int abzug = 0;
        if (r == extra) abzug = extra->getAnzahl();
        QList<int> *liste = aktuellerWagen->besetzePlaetze(r);
        Wagen *alt = aktuellerWagen;
        if (aktuellerWagen->getFreiePlaetze() == 0) {
            if (wagen->indexOf(aktuellerWagen) == wagen->length()-1) {
                aktuellerWagen = nullptr;
            } else {
                aktuellerWagen = wagen->at(wagen->indexOf(aktuellerWagen)+1);
            }
        }
        double bewertungNeu = bewertung + anzahlToBewertung.value(sortiert.at(j));
        QSet<Reservierung*> rest2 = rest;
        rest2.remove(r);

        verteile(bewertungNeu, rest2, puffer-abzug);

        aktuellerWagen = alt;
        alt = nullptr;
        r = nullptr;
        aktuellerWagen->verlassePlaetze(liste);
        delete liste;
    }
    delete extra;
}


void Verteiler::weisePlaetzeZu()
{
    for(Wagen *w: *wagen) {
        w->weisePlaetzeZu();
    }
}

void Verteiler::setCheckAll(bool value)
{
    checkAll = value;
}
