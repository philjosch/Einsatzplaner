#include "verteiler.h"

#include <QDebug>
#include <QStack>
#include <QTime>

Verteiler::Verteiler(QList<Wagen *> wagen, QSet<Reservierung *> reservierungen)
{
    this->wagen = wagen;
    this->reservierungen = reservierungen;
    found = false;
    besteBewertung = 1215752192;
    aktWg = 0;
    count = 0;
    checkAll = false;
}

bool Verteiler::verteile()
{
    return false;
    // berechne untere Schranke für die Güte dees ergebnisses
    int puffer = 0;
    for (Wagen *w: wagen) {
        puffer += w->getKapazitaet();
    }
    for (Reservierung *r: reservierungen) {
        puffer -= r->getAnzahl();
    }
    if (puffer < 0) return false;
    verteile(0, reservierungen, puffer);

    return found;
}

void Verteiler::verteile(double bewertung, QSet<Reservierung *> rest, int puffer)
{
    if (bewertung > besteBewertung) return;

    // Alles wurde Verteilt
    if (rest.isEmpty()) {
        count ++;
        weisePlaetzeZu();
        besteBewertung = bewertung;
        found = true;
        return;
    }

    if (aktWg >= wagen.size()) return;

    // Berechne den kleinsten Fehler und prüfe, ob Ziel noch erreicht werden kann
    if (bewertung + getMinBewertung(rest) > besteBewertung) return;

    Wagen *aktuellerWagen = wagen.at(aktWg);

    QHash<int, Reservierung*> anzahlToReservierung;
    QHash<double, QList<int>> bewertungDerReservierungen;
    for(Reservierung *r: rest.values()) {

        if (anzahlToReservierung.contains(r->getAnzahl())) continue;
        if (aktuellerWagen->getAnzahlFrei() < r->getAnzahl()) continue;

        double bewertungReservierung = aktuellerWagen->getStrafpunkteFuerPlaetze(r->getAnzahl());

        if (bewertung + bewertungReservierung < besteBewertung) {
            anzahlToReservierung.insert(r->getAnzahl(), r);
            if (! bewertungDerReservierungen.contains(bewertungReservierung)) {
                bewertungDerReservierungen.insert(bewertungReservierung, QList<int>());
            }
            QList<int> help = bewertungDerReservierungen.value(bewertungReservierung);
            help.append(r->getAnzahl());
            bewertungDerReservierungen.insert(bewertungReservierung, help);
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
            bewertungDerReservierungen.insert(bewertungExtra, QList<int>());
        QList<int> help = bewertungDerReservierungen.value(bewertungExtra);
        help.append(freiePlaetze);
        bewertungDerReservierungen.insert(bewertungExtra, help);
    }

    if(anzahlToReservierung.isEmpty()) return;

    // Sortiere die Liste mit Reservierungen zuerst nach Bewertung und 2. rangig nach Anzahl absteigend
    QList<double> keys = bewertungDerReservierungen.keys();
    qSort(keys);
    QList<int> sortiert;
    QHash<int,double> anzahlToBewertung;
    if (checkAll) {
        for (double d: keys) {
            QList<int> liste = bewertungDerReservierungen.value(d);
            qSort(liste);
            for(int i = liste.length()-1; i >= 0; --i) {
                sortiert.append(liste.at(i));
                anzahlToBewertung.insert(liste.at(i), d);
            }
        }
    } else {
        double d = keys.first();
        QList<int> liste = bewertungDerReservierungen.value(d);
        qSort(liste);
        for(int i = liste.length()-1; i >= 0; --i) {
            sortiert.append(liste.at(i));
            anzahlToBewertung.insert(liste.at(i), d);
        }
//        if (keys.length() >= 2) {
//            d = keys.at(1);
//            liste = bewertungDerReservierungen.value(d);
//            qSort(liste);
//            for(int i = liste.length()-1; i >= 0; --i) {
//                sortiert.append(liste.at(i));
//                anzahlToBewertung.insert(liste.at(i), d);
//            }
//        }

    }
    bewertungDerReservierungen.clear();

    Reservierung *r = nullptr;
    int alt = aktWg;
    for(int j = 0; j < sortiert.length() /* && (checkAll || ! found) */; j++) {
        // wenn es perfekt passende Plaetze gibt, prüfe nur diese!
//        if ((anzahlToBewertung.value(sortiert.first()) == 0) && (anzahlToBewertung.value(sortiert.at(j)) != 0)) {
//            qDebug() << "Hier";
//            break;
//        }
        r = anzahlToReservierung.value(sortiert.at(j));
        QList<int> liste = aktuellerWagen->besetzePlaetze(r);
        if (aktuellerWagen->getAnzahlFrei() == 0) {
            aktWg++;
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

        if (bewertungNeu + getMinBewertung(restNeu) < besteBewertung) {
            verteile(bewertungNeu, restNeu, pufferNeu);
        }
        aktWg = alt;
        aktuellerWagen->verlassePlaetze(liste);
    }
    r = nullptr;
    delete extra;
}

bool Verteiler::verteile2()
{
    // berechne untere Schranke für die Güte dees ergebnisses
    int puffer = 0;
    for (Wagen *w: wagen) {
        puffer += w->getKapazitaet();
    }
    for (Reservierung *r: reservierungen) {
        puffer -= r->getAnzahl();
    }
    if (puffer < 0) return false;


    QStack<Configuration> stack{};
    // Initialisierung
    ResBewTuple tuple = getNaechsteReservierungen(reservierungen, aktWg, 0);
    for(int i = tuple.res.length()-1; i >= 0; --i) {
        QSet<Reservierung*> res = reservierungen;
        res.remove(tuple.res.at(i));
        stack.push(Configuration{tuple.bew.at(i), res, puffer, aktWg, tuple.res.at(i), QList<int>()});
    }

    int aktWg = 0;
    Reservierung *r;
    ResBewTuple sortiert;
    while(! stack.isEmpty()) {
        Configuration config = stack.pop();
        if (! config.sitze.isEmpty()) {
            wagen.at(config.aktWagen)->verlassePlaetze(config.sitze);
            continue;
        }
        config.sitze = wagen.at(config.aktWagen)->besetzePlaetze(config.res);
        stack.push(config);

//        if (config.bewertung > besteBewertung) continue;

        if (config.rest.isEmpty()) {
            count ++;
            weisePlaetzeZu();
            besteBewertung = config.bewertung;
            found = true;
            continue;
        }

        aktWg = config.aktWagen;
        if (wagen.at(aktWg)->getAnzahlFrei() == 0) aktWg++;

        if (aktWg >= wagen.length()) continue;
        if (config.bewertung + getMinBewertung(config.rest) > besteBewertung) continue;

        sortiert = getNaechsteReservierungen(config.rest, aktWg, config.bewertung, config.puffer);

        for(int i = sortiert.res.length()-1; i >= 0; --i) {
            Configuration newConfig;

            r = sortiert.res.at(i);
            newConfig.bewertung = config.bewertung;
            newConfig.rest = config.rest;
            newConfig.puffer = newConfig.puffer;
            newConfig.aktWagen = aktWg;
            newConfig.res = r;
            newConfig.sitze = QList<int>();

            newConfig.bewertung += sortiert.bew.at(i);
            newConfig.rest.remove(r);

            stack.push(newConfig);
        }
    }

    return found;
}

void Verteiler::weisePlaetzeZu()
{
    for(Wagen *w: wagen) {
        w->weisePlaetzeZu();
    }
}

void Verteiler::setCheckAll(bool value)
{
    checkAll = value;
}

double Verteiler::getMinBewertung(QSet<Reservierung *> liste)
{
    double summe = 0.0;
    for(Reservierung *r: liste.values()) {
        double min = 1215752192;
        for(int i = aktWg; i < wagen.length(); ++i) {
            Wagen *w = wagen.at(i);
            for(int start = 0; start < w->getAnzahlFrei(); ++start) {
                double bew = w->getStrafpunkteFuerPlaetze(r->getAnzahl(), start);
                if (bew < min && bew >= 0)
                    min = bew;
            }
        }
        summe += min;
    }
    return summe;
}

Verteiler::ResBewTuple Verteiler::getNaechsteReservierungen(QSet<Reservierung *> res, int aktWagen, double bewertung, int puffer)
{
    ResBewTuple werte = {QList<Reservierung*>(), QList<double>(), nullptr};
    Wagen *aktuellerWagen = wagen.at(aktWagen);

    QList<Reservierung*> listRes;
    QList<double> listBew;
    QSet<int> setAnzahl;
    foreach(Reservierung *r, res) {
        if (setAnzahl.contains(r->getAnzahl())) continue;
        if (aktuellerWagen->getAnzahlFrei() < r->getAnzahl()) continue;

        double bewertungRes = aktuellerWagen->getStrafpunkteFuerPlaetze(r->getAnzahl());
        if (bewertung + bewertungRes >= besteBewertung) continue;
        setAnzahl.insert(r->getAnzahl());
        listBew.append(bewertungRes);
        listRes.append(r);

        // Element korrekt einsortieren
        // Zuerst nach der Bewertung und dann nach der Anzahl
        int i = listRes.length()-1;
        while (i > 0) {
            if (listBew.at(i-1) > listBew.at(i)) {
                listBew.swap(i-1, i);
                listRes.swap(i-1, i);
                i--;
            } else if (qFuzzyCompare(listBew.at(i-1)+1.0, listBew.at(i)+1.0)) {
                if (listRes.at(i-1)->getAnzahl() < listRes.at(i)->getAnzahl()) {
                    listBew.swap(i-1, i);
                    listRes.swap(i-1, i);
                    i--;
                } else {
                    break;
                }
            } else {
                break;
            }
        }
    }
    // TODO: Extra Reservierung mit verwendung des Puffers einfuegen

    if (! checkAll) {
        QList<Reservierung*> listRes2;
        QList<double> listBew2;

        for(int i = 0; i < listRes.length() && i < 2; i++) {
            listRes2.append(listRes.at(i));
            listBew2.append(listBew.at(i));
        }
        listBew = listBew2;
        listRes = listRes2;
    }

    werte.bew = listBew;
    werte.res = listRes;

    return werte;
}
