#include "wagen.h"

#include <math.h>
#include <cmath>

#include <QDebug>

double Wagen::FAKTOR_FUER_FREIE_PLAETZE = 0.9;
double Wagen::GEWICHT_STRAFE_TEIL_1 = 10.0;
double Wagen::GEWICHT_STRAFE_TEIL_2 = 3.0;

Wagen::Wagen(int nummer)
{
    aktuellePosition = 0;
    this->nummer = nummer;

    sitzGruppen = new QList<int>();
    verteilung = new QList<Reservierung*>();

    switch (nummer) {
    case 208:
        size = 60;
        reihe1 = 2;
        reihe2 = 3;
        for (int i =  0; i <  5; i++) { sitzGruppen->append(1); } // 5
        for (int i =  5; i <  7; i++) { sitzGruppen->append(2); } // 2
        for (int i =  7; i < 11; i++) { sitzGruppen->append(3); } // 4
        for (int i = 11; i < 17; i++) { sitzGruppen->append(4); } // 6
        for (int i = 17; i < 23; i++) { sitzGruppen->append(5); } // 6
        for (int i = 23; i < 27; i++) { sitzGruppen->append(6); } // 4
        for (int i = 27; i < 31; i++) { sitzGruppen->append(7); } // 4
        for (int i = 31; i < 37; i++) { sitzGruppen->append(8); } // 6
        for (int i = 37; i < 43; i++) { sitzGruppen->append(9); } // 6
        for (int i = 43; i < 47; i++) { sitzGruppen->append(10); } // 4
        for (int i = 47; i < 51; i++) { sitzGruppen->append(11); } // 4
        for (int i = 51; i < 56; i++) { sitzGruppen->append(12); } // 5
        for (int i = 56; i < 58; i++) { sitzGruppen->append(13); } // 2
        for (int i = 58; i < 60; i++) { sitzGruppen->append(14); } // 2
        // C3YG
        break;
    case 202:
        size = 62;
        reihe1 = 2;
        reihe2 = 3;
        for (int i =  0; i <  5; i++) { sitzGruppen->append(1); } // 5
        for (int i =  5; i <  7; i++) { sitzGruppen->append(2); } // 2
        for (int i =  7; i < 11; i++) { sitzGruppen->append(3); } // 4
        for (int i = 11; i < 17; i++) { sitzGruppen->append(4); } // 6
        for (int i = 17; i < 23; i++) { sitzGruppen->append(5); } // 6
        for (int i = 23; i < 27; i++) { sitzGruppen->append(6); } // 4
        for (int i = 27; i < 31; i++) { sitzGruppen->append(7); } // 4
        for (int i = 31; i < 37; i++) { sitzGruppen->append(8); } // 6
        for (int i = 37; i < 43; i++) { sitzGruppen->append(9); } // 6
        for (int i = 43; i < 47; i++) { sitzGruppen->append(10); } // 4
        for (int i = 47; i < 51; i++) { sitzGruppen->append(11); } // 4
        for (int i = 51; i < 55; i++) { sitzGruppen->append(12); } // 4
        for (int i = 55; i < 60; i++) { sitzGruppen->append(13); } // 5
        for (int i = 60; i < 62; i++) { sitzGruppen->append(14); } // 2
        // B3YG
        break;
    case 201:
        size = 33;
        reihe1 = 2;
        reihe2 = 2;
        for (int i =  0; i <  4; i++) { sitzGruppen->append(1); } // 4
        for (int i =  4; i < 10; i++) { sitzGruppen->append(2); } // 6
        for (int i = 10; i < 16; i++) { sitzGruppen->append(3); } // 6
        for (int i = 16; i < 20; i++) { sitzGruppen->append(4); } // 4
        for (int i = 20; i < 24; i++) { sitzGruppen->append(5); } // 4
        for (int i = 24; i < 30; i++) { sitzGruppen->append(6); } // 6
        for (int i = 30; i < 33; i++) { sitzGruppen->append(7); } // 3
        // BD3YG
        break;

    case 204:
        size = 40;
        reihe1 = 2;
        reihe2 = 2;
        for (int i =  0; i <  4; i++) { sitzGruppen->append(1); } // 4
        for (int i =  4; i <  8; i++) { sitzGruppen->append(2); } // 4
        for (int i =  8; i < 12; i++) { sitzGruppen->append(3); } // 4
        for (int i = 12; i < 16; i++) { sitzGruppen->append(4); } // 4
        for (int i = 16; i < 20; i++) { sitzGruppen->append(5); } // 4
        for (int i = 20; i < 24; i++) { sitzGruppen->append(6); } // 4
        for (int i = 24; i < 28; i++) { sitzGruppen->append(7); } // 4
        for (int i = 28; i < 32; i++) { sitzGruppen->append(8); } // 4
        for (int i = 32; i < 36; i++) { sitzGruppen->append(9); } // 4
        for (int i = 36; i < 40; i++) { sitzGruppen->append(10); } // 4
        // WG3yG alt
        break;

    case 217:
        size = 40;
        reihe1 = 2;
        reihe2 = 2;
        for (int i =  0; i <  4; i++) { sitzGruppen->append(1); } // 4
        for (int i =  4; i <  8; i++) { sitzGruppen->append(2); } // 4
        for (int i =  8; i < 12; i++) { sitzGruppen->append(3); } // 4
        for (int i = 12; i < 16; i++) { sitzGruppen->append(4); } // 4
        for (int i = 16; i < 20; i++) { sitzGruppen->append(5); } // 4
        for (int i = 20; i < 24; i++) { sitzGruppen->append(6); } // 4
        for (int i = 24; i < 28; i++) { sitzGruppen->append(7); } // 4
        for (int i = 28; i < 32; i++) { sitzGruppen->append(8); } // 4
        for (int i = 32; i < 36; i++) { sitzGruppen->append(9); } // 4
        for (int i = 36; i < 40; i++) { sitzGruppen->append(10); } // 4
        // WG3YG neu
        break;
    default:
        size = 0;
        reihe1 = 0;
        reihe2 = 0;
    }
}

int Wagen::getNummer() const
{
    return nummer;
}

int Wagen::klasse(int nummer)
{
    switch(nummer) {
    case 217: return 1;
    case 204: return 1;
    case 201: return 2;
    case 202: return 2;
    case 208: return 3;
    default: return 0;
    }
}

int Wagen::getAnzahlFreiePlaetzeInSitzgruppe()
{
    if (aktuellePosition == sitzGruppen->length()) return 0;
    int ende = sitzGruppen->lastIndexOf(sitzGruppen->at(aktuellePosition));
    return ende - aktuellePosition + 1;
}

void Wagen::verlassePlaetze(QList<int> *liste)
{
    for(int i = 0; i < liste->length(); i++) {
        verteilung->removeLast();
        aktuellePosition--;
    }
}

QList<int> *Wagen::besetzePlaetze(Reservierung *r)
{
    QList<int> *liste = new QList<int>();
    for(int i = 0; i < r->getAnzahl(); i++) {
        verteilung->append(r);
        liste->append(aktuellePosition);
        aktuellePosition++;
    }
    return liste;
}

double Wagen::getStrafpunkteFuerPlaetze(int anzahl, int start)
{
    // Vorbereitungen
    if (start == -1) start = aktuellePosition;

    int ende = start + anzahl - 1;
    if (ende >= size) return -1; // wenn die Plätze nicht in diesem Wagen platziert werden können

    int sitzGruppeStart = sitzGruppen->at(start);
    int sitzGruppeEnde = sitzGruppen->at(ende);

    int beginnErste = sitzGruppen->indexOf(sitzGruppeStart);
    int endeErste = sitzGruppen->lastIndexOf(sitzGruppeStart);

    int beginnLetzte = sitzGruppen->indexOf(sitzGruppeEnde);
    int endeLetzte = sitzGruppen->lastIndexOf(sitzGruppeEnde);

    // Berechnung der Strafpunkte
    double sp1 = 0;
    double sp2 = 0;

     // Sp 1 //
    double maxB = ceil(anzahl / fmax(2*reihe1, 2*reihe2));

    // Bestimme die belegten Sitzgruppen
    double benoetigt = sitzGruppeEnde - sitzGruppeStart + 1;
    sp1 = benoetigt - maxB;

    // Sp 2 //
    int ueV = start - beginnErste;; // Anzahl der durch fremde Reservierungen belegten Plätze am Anfang
    int sgV = endeErste - beginnErste + 1; // Anzahl der Plaetze in der ersten Sitzgruppe
    int ueH = endeLetzte - ende; // Anzahl der durch fremde Reservierungen belegten Sitzplaetze in der letzten Sitzgruppe
    int sgH = endeLetzte - beginnLetzte + 1; // Anzahl der Plaetze in letzter Sitzgruppe

    if (ueV != 0){
        //         Fehler, weil Sitzgruppe nciht zu Hälte belegt ist
        //                                                  Fehler weil Sitzgruppe nicht komplett belegt ist
//        sp2 += /* 1 + */ ceil(std::abs(sgV * 0.5 - ueV)) * 0.5 + (sgV - ueV) * 0.5;
    }
    if ((sitzGruppen->at(start) != sitzGruppen->at(ende)) && (ueH != 0)){
        // Die gleichen Fehlerpunkte, wenn dies auch bei der letzten Sitzgruppe zutrifft und sie verschieden von der ersten ist
//        sp2 += /* 1 + */ ceil(std::abs(sgH * 0.5 - ueH)) * 0.5 + (sgH - ueH) * 0.5;
    }

    if (sitzGruppen->at(start) != sitzGruppen->at(ende)) {
        sp2 += ueV*ueV + ueH *ueH;
    } else {
        sp2 += ueV*ueV;
    }

    // Gewichtung der einzelnen Strafpunkte //
    return GEWICHT_STRAFE_TEIL_1 * sp1 + GEWICHT_STRAFE_TEIL_2 * sp2;
}

int Wagen::getFreiePlaetze()
{
    return size-aktuellePosition;
}

bool Wagen::isEmpty()
{
    return aktuellePosition == 0;
}



void Wagen::print()
{
    for(int i = 0; i < verteilung->length(); ++i) {
//        qDebug() << i;
//        qDebug() << verteilung->at(i)->getName();
    }
}

void Wagen::weisePlaetzeZu()
{
    QMap<Reservierung*, QList<int>*> liste;
    for(int i = 0; i < verteilung->length(); ++i) {
        Reservierung *r = verteilung->at(i);
        if (!liste.contains(r))
            liste.insert(r, new QList<int>());
        liste.value(r)->append(i+1);
    }

    for(Reservierung *r: liste.keys()) {
        QMap<int, QList<int>*> *plaetze = new QMap<int, QList<int>*>();
        plaetze->insert(nummer, liste.value(r));
        r->setSitzplatz(plaetze);
    }

}
