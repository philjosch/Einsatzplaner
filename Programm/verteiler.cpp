#include "verteiler.h"

#include <QStack>

Verteiler::Verteiler(QList<Wagen *> wagen, QSet<Reservierung *> reservierungen, bool all)
{
    this->wagen = wagen;
    this->reservierungen = reservierungen;
    found = false;
    besteBewertung = 1215752192;
    aktWg = 0;
    count = 0;
    checkAll = all;
}

Mistake Verteiler::verteile()
{
    // berechne untere Schranke für die Güte dees ergebnisses
    int puffer = 0;
    for (Wagen *w: wagen) {
        puffer += w->getKapazitaet();
    }
    for (Reservierung *r: reservierungen) {
        puffer -= r->getAnzahl();
    }
    if (puffer < 0) return Mistake::KapazitaetUeberlauf;


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
            newConfig.puffer = config.puffer;
            if (sortiert.extra == r) {
                newConfig.puffer -= r->getAnzahl();
                newConfig.bewertung -= sortiert.bew.at(i);
            }

            newConfig.aktWagen = aktWg;
            newConfig.res = r;
            newConfig.sitze = QList<int>();

            newConfig.bewertung += sortiert.bew.at(i);
            newConfig.rest.remove(r);

            stack.push(newConfig);
        }
    }

    if (found) return Mistake::OK;
    else return Mistake::SonstigerFehler;
}

void Verteiler::weisePlaetzeZu()
{
    for(Wagen *w: wagen) {
        w->weisePlaetzeZu();
    }
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
                listBew.swapItemsAt(i-1, i);
                listRes.swapItemsAt(i-1, i);
                i--;
            } else if (qFuzzyCompare(listBew.at(i-1)+1.0, listBew.at(i)+1.0)) {
                if (listRes.at(i-1)->getAnzahl() < listRes.at(i)->getAnzahl()) {
                    listBew.swapItemsAt(i-1, i);
                    listRes.swapItemsAt(i-1, i);
                    i--;
                } else {
                    break;
                }
            } else {
                break;
            }
        }
    }

    int missing = aktuellerWagen->getAnzahlFreiePlaetzeInSitzgruppe();
    Reservierung *extra = nullptr;
    if ((! setAnzahl.contains(missing)) && 0 < missing && missing <= puffer) {
        double bewertungRes = aktuellerWagen->getStrafpunkteFuerPlaetze(missing);
        if (bewertung + bewertungRes < besteBewertung) {
            setAnzahl.insert(missing);
            listBew.append(bewertungRes);
            extra = new Reservierung(nullptr);
            extra->setAnzahl(missing);
            listRes.append(extra);

            // Element korrekt einsortieren
            // Zuerst nach der Bewertung und dann nach der Anzahl
            int i = listRes.length()-1;
            while (i > 0) {
                if (listBew.at(i-1) > listBew.at(i)) {
                    listBew.swapItemsAt(i-1, i);
                    listRes.swapItemsAt(i-1, i);
                    i--;
                } else if (qFuzzyCompare(listBew.at(i-1)+1.0, listBew.at(i)+1.0)) {
                    if (listRes.at(i-1)->getAnzahl() < listRes.at(i)->getAnzahl()) {
                        listBew.swapItemsAt(i-1, i);
                        listRes.swapItemsAt(i-1, i);
                        i--;
                    } else {
                        break;
                    }
                } else {
                    break;
                }
            }
        }
    }

    if (! checkAll) {
        QList<Reservierung*> listRes2;
        QList<double> listBew2;

        for(int i = 0; i < listRes.length() && (i < 2 || (i < 3 && listRes2.contains(extra))); i++) {
            listRes2.append(listRes.at(i));
            listBew2.append(listBew.at(i));
        }
        listBew = listBew2;
        listRes = listRes2;
    }

    werte.bew = listBew;
    werte.res = listRes;
    werte.extra = extra;

    return werte;
}
