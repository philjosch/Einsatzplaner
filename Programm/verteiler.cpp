#include "verteiler.h"

#include <QDebug>
#include <QTime>

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
    int puffer = 0;
    for (Wagen *w: *wagen) {
        puffer += w->getKapazitaet();
    }
    for (Reservierung *r: *reservierungen) {
        puffer -= r->getAnzahl();
    }
    if (puffer < 0) return false;
    mindestbewertung = getMinBewertung(reservierungen);
    verteile(0, *reservierungen, puffer);
    return found;
}

void Verteiler::verteile(double bewertung, QSet<Reservierung *> rest, int puffer)
{
    if (bewertung > besteBewertung) {
        return;
    }
    // Berechne den kleinsten Fehler und prüfe, ob Ziel noch erreicht werden kann
    if (bewertung + getMinBewertung(&rest) > besteBewertung) {
        return;
    }

    // Alles wurde Verteilt
    if (rest.isEmpty()) {
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
        if (aktuellerWagen->getAnzahlFrei() < r->getAnzahl()) continue;

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
    Reservierung *extra = new Reservierung(nullptr);
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
    if (checkAll) {
        for (double d: keys) {
            QList<int> *liste = bewertungDerReservierungen.value(d);
            qSort(*liste);
            for(int i = liste->length()-1; i >= 0; --i) {
                sortiert.append(liste->at(i));
                anzahlToBewertung.insert(liste->at(i), d);
            }
            delete liste;
        }
    } else {
        double d = keys.first();
        QList<int> *liste = bewertungDerReservierungen.value(d);
        qSort(*liste);
        for(int i = liste->length()-1; i >= 0; --i) {
            sortiert.append(liste->at(i));
            anzahlToBewertung.insert(liste->at(i), d);
        }
        if (keys.length() >= 2) {
            d = keys.at(1);
            liste = bewertungDerReservierungen.value(d);
            qSort(*liste);
            for(int i = liste->length()-1; i >= 0; --i) {
                sortiert.append(liste->at(i));
                anzahlToBewertung.insert(liste->at(i), d);
            }
        }
        delete liste;

    }
    bewertungDerReservierungen = QHash<double,QList<int>*>();

    for(int j = 0; j < sortiert.length() /* && (checkAll || ! found) */; j++) {
        // wenn es perfekt passende Plaetze gibt, prüfe nur diese!
        if ((anzahlToBewertung.value(sortiert.first()) == 0) && (anzahlToBewertung.value(sortiert.at(j)) != 0)) {
            break;
        }
        Reservierung *r = anzahlToReservierung.value(sortiert.at(j));
        QList<int> *liste = aktuellerWagen->besetzePlaetze(r);
        Wagen *alt = aktuellerWagen;
        if (aktuellerWagen->getAnzahlFrei() == 0) {
            if (wagen->indexOf(aktuellerWagen) == wagen->length()-1) {
                aktuellerWagen = nullptr;
            } else {
                aktuellerWagen = wagen->at(wagen->indexOf(aktuellerWagen)+1);
            }
        }
        double bewertungNeu = bewertung;
        int pufferNeu = puffer;
        if (r == extra) {
            pufferNeu -= extra->getAnzahl();
        } else {
            bewertungNeu += anzahlToBewertung.value(sortiert.at(j));
        }
        QSet<Reservierung*> restNeu = rest;
        restNeu.remove(r);

        if (bewertungNeu + getMinBewertung(&restNeu) <= besteBewertung) {
            verteile(bewertungNeu, restNeu, pufferNeu);
        }
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

double Verteiler::getMinBewertung(QSet<Reservierung *> *liste)
{
    double summe = 0.0;
    for(Reservierung *r: liste->values()) {
        double min = 1215752192;
        if (aktuellerWagen != nullptr) {
            for(int i = wagen->indexOf(aktuellerWagen); i < wagen->length(); ++i) {
                Wagen *w = wagen->at(i);
                for(int start = 0; start < w->getAnzahlFrei(); ++start) {
                    double bew = w->getStrafpunkteFuerPlaetze(r->getAnzahl(), start);
                    if ((bew < min) && (bew != -1))
                        min = bew;
                }
            }
        }
        summe += min;
    }
    return summe;
}
