#include "verteiler.h"
#include <QDebug>
#include <QListWidgetItem>

Verteiler::Verteiler(QList<Wagen *> *wagen, QList<Reservierung *> *reservierungen)
{
    this->wagen = wagen;
    this->reservierungen = reservierungen;
    found = false;
    obereGrenze = 1215752192;
    aktuellerWagen = 0;
    int maximal = 0;
    foreach (Wagen *w, *wagen) {
        maximal += w->getFreiePlaetze();
    }
    puffer = maximal-reservierungen->length();

    rest = new QSet<int>();
    for (int i = 0; i < reservierungen->length(); ++i) {
        rest->insert(i);
    }
    count = 0;

}

bool Verteiler::verteile()
{
    if (rest->isEmpty()) return false;
    verteile(0);
    qDebug("%d", count);
/*    foreach (Reservierung *res, *reservierungen) {
        res->takePlatz();
    }*/
    return found;
}

void Verteiler::verteile(int bewertung)
{
    count ++;
    if (bewertung > obereGrenze) {
        return;
    }

    if (rest->isEmpty()) {
        qDebug("gefunden");
        foreach (Reservierung *res, *reservierungen) {
            res->takePlatz();
        }
        if (bewertung == obereGrenze) {
            found = true;
        }
        obereGrenze = bewertung;
//        found = true;
        return;
    }

    // Vorangestellt Auswahl der Resrvierungen, die als nächsten eingefügt werden können
    QList<Reservierung *> *lokaleListe = new QList<Reservierung*>();
    QSet<int> *anzahl = new QSet<int>();
    QList<double> *einzelneBewertung = new QList<double>();
    foreach(const int i, *rest) {
        Reservierung *res = reservierungen->at(i);

        if (anzahl->contains(res->getAnzahl())) continue;
        anzahl->insert(res->getAnzahl()); // Keine Alternativen verfolgen, die äquivalente Belegungen erzeugen

        if (! platziere(res)) continue;

        int a = gradeYes(res);
        if ((bewertung + a) < obereGrenze) {
            einzelneBewertung->append(a);
            lokaleListe->append(res);
            for (int j = einzelneBewertung->length()-1; j > 0 &&
                 (einzelneBewertung->at(j-1) > einzelneBewertung->at(j) ||
                  (einzelneBewertung->at(j-1) == einzelneBewertung->at(j) && lokaleListe->at(j-1)->getAnzahl() < lokaleListe->at(j)->getAnzahl())
                  ); j--) {
                lokaleListe->swap(j, j-1);
                einzelneBewertung->swap(j, j-1);
            }
        }
        dePlatziere(res);
    }
    if (einzelneBewertung->isEmpty()) return;

    Reservierung *extra = new Reservierung(NULL);
    extra->setName("EXTRA!-!-!-");
    extra->setAnzahl(0);
    if (einzelneBewertung->length() > 0 && einzelneBewertung->first() >= 0.1) {
        // Es gibt kein Element, dass passt
        int a = wagen->at(aktuellerWagen)->getAnzahlBisEnde();
        if (a <= puffer && ! anzahl->contains(a)) {
            puffer -= a;
            extra->setAnzahl(a);
            lokaleListe->insert(0, extra);
            einzelneBewertung->insert(0, a*0.9);
            reservierungen->append(extra);
            rest->insert(reservierungen->length()-1);
            int j = 0;
            while (j < einzelneBewertung->length()-1 && einzelneBewertung->at(j+1) < einzelneBewertung->at(j)) {
                einzelneBewertung->swap(j, j+1);
                lokaleListe->swap(j, j+1);
                j++;
            }
        }
    }
/*    QString s = "%d ";
    for (int i = 0; i < lokaleListe->length(); ++i) {
       s += ", "+lokaleListe->at(i)->getName() +QString::number(lokaleListe->at(i)->getAnzahl()) +"-"+QString::number(einzelneBewertung->at(i));
    }
    qDebug() << s;
*/

    // Die Reservierungen wirklich verteilen
    for (int i = 0; i < lokaleListe->length() && !found; ++i) {
        Reservierung *res = lokaleListe->at(i);
        if (platziere(res)) {
//            double a = gradeYes(res);
//            bewertung += a;
            int index = reservierungen->indexOf(res);
            rest->remove(index);
            verteile(bewertung+gradeYes(res));
//            bewertung -= a;
            dePlatziere(res);
            rest->insert(index);
        }
        if (extra == res) {
            rest->remove(reservierungen->length()-1);
            reservierungen->removeLast();
            puffer += extra->getAnzahl();
        }

    }
}

double Verteiler::gradeYes(Reservierung *res) {
    return res->getWagen()->getStrafpunkte(res->getPlaetze());
}

double Verteiler::gradeNo(Reservierung *res) {
    // ACHTUNG NUR FÜR 204 und 217 geeignet!?!?
//    double summe = minimum((4 - ((res->getAnzahl()%10) % 4))% 4, (6 - ((res->getAnzahl()%10) % 6))%6);
    // Konservative Schranke nach unten, mindestens 1/5 der Plätze wird auf die "falsche" Seite gesetzt
    double summe = (double(res->getAnzahl()) * 0.4);
    return summe;
}

int Verteiler::minimum(int a, int b) {
    if (a < b) return a;
    return b;
}

bool Verteiler::platziere(Reservierung *res)
{
    if (aktuellerWagen < wagen->length() && wagen->at(aktuellerWagen)->getFreiePlaetze() >= res->getAnzahl()) {
        res->setPlaetze(wagen->at(aktuellerWagen), wagen->at(aktuellerWagen)->besetze(res));
        if (wagen->at(aktuellerWagen)->getFreiePlaetze() == 0) {
            ++aktuellerWagen;
        }
        return true;
    }
    return false;
}

void Verteiler::dePlatziere(Reservierung *res)
{
    if (aktuellerWagen >= wagen->length() || (wagen->at(aktuellerWagen)->isEmpty() && aktuellerWagen > 0)) {
        --aktuellerWagen;
    }
    wagen->at(aktuellerWagen)->verlasse(res->getPlaetze());
/*    if ((wagen->at(aktWagen)->isEmpty() && aktWagen > 0)) {
        --aktWagen;
    }*/
}

