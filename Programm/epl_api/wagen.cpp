#include "wagen.h"

#include <cmath>

double Wagen::GEWICHT_STRAFE_TEIL_1 = 10.0;
double Wagen::GEWICHT_STRAFE_TEIL_2 = 11.0;

Wagen::Wagen(int nummer)
{
    aktuellePosition = 0;
    this->nummer = nummer;

    sitzGruppen = QList<int>();
    verteilung = QHash<int, Reservierung*>();
    internToExtern = QList<int>();
    // Reihe1 = Oben (Nord)
    // Reihe2 = Unten (Süden)
    // Nummerrierung von Links nach Rechts (West nach Ost)
    // Wagen stehen im Heimatbahnhof

    switch (nummer) {
    case 208:
        size = 60;
        reihe1 = 3;
        reihe2 = 2;
        for (int i =  0; i <  5; i++) { sitzGruppen.append(1); } // 5
        for (int i =  5; i <  7; i++) { sitzGruppen.append(2); } // 2
        for (int i =  7; i < 11; i++) { sitzGruppen.append(3); } // 4
        for (int i = 11; i < 17; i++) { sitzGruppen.append(4); } // 6
        for (int i = 17; i < 23; i++) { sitzGruppen.append(5); } // 6
        for (int i = 23; i < 27; i++) { sitzGruppen.append(6); } // 4
        for (int i = 27; i < 31; i++) { sitzGruppen.append(7); } // 4
        for (int i = 31; i < 37; i++) { sitzGruppen.append(8); } // 6
        for (int i = 37; i < 43; i++) { sitzGruppen.append(9); } // 6
        for (int i = 43; i < 47; i++) { sitzGruppen.append(10); } // 4
        for (int i = 47; i < 51; i++) { sitzGruppen.append(11); } // 4
        for (int i = 51; i < 56; i++) { sitzGruppen.append(12); } // 5
        for (int i = 56; i < 58; i++) { sitzGruppen.append(13); } // 2
        for (int i = 58; i < 60; i++) { sitzGruppen.append(14); } // 2
        internToExtern << 58 << 57 << 54 << 55 << 56; // 1
        internToExtern << 60 << 59; // 2
        internToExtern << 52 << 53 << 50 << 51; // 3
        internToExtern << 49 << 48 << 47 << 44 << 45 << 46; // 4
        internToExtern << 37 << 38 << 39 << 34 << 35 << 36; // 5
        internToExtern << 43 << 42 << 41 << 40; // 6
        internToExtern << 32 << 33 << 30 << 31; // 7
        internToExtern << 29 << 28 << 27 << 24 << 25 << 26; // 8
        internToExtern << 17 << 18 << 19 << 14 << 15 << 16; // 9
        internToExtern << 23 << 22 << 21 << 20; // 10
        internToExtern << 12 << 13 << 10 << 11; // 11
        internToExtern <<  9 <<  8 <<  7 <<  5 <<  6; // 12
        internToExtern <<  1 <<  2; // 13
        internToExtern <<  3 <<  4; // 14
        // C3YG
        break;
    case 202:
        size = 62;
        reihe1 = 3;
        reihe2 = 2;
        for (int i =  0; i <  5; i++) { sitzGruppen.append(1); } // 5
        for (int i =  5; i <  7; i++) { sitzGruppen.append(2); } // 2
        for (int i =  7; i < 11; i++) { sitzGruppen.append(3); } // 4
        for (int i = 11; i < 17; i++) { sitzGruppen.append(4); } // 6
        for (int i = 17; i < 23; i++) { sitzGruppen.append(5); } // 6
        for (int i = 23; i < 27; i++) { sitzGruppen.append(6); } // 4
        for (int i = 27; i < 31; i++) { sitzGruppen.append(7); } // 4
        for (int i = 31; i < 37; i++) { sitzGruppen.append(8); } // 6
        for (int i = 37; i < 43; i++) { sitzGruppen.append(9); } // 6
        for (int i = 43; i < 47; i++) { sitzGruppen.append(10); } // 4
        for (int i = 47; i < 51; i++) { sitzGruppen.append(11); } // 4
        for (int i = 51; i < 57; i++) { sitzGruppen.append(12); } // 6
        for (int i = 57; i < 60; i++) { sitzGruppen.append(13); } // 3
        for (int i = 60; i < 62; i++) { sitzGruppen.append(14); } // 2
        internToExtern << 59 << 60 << 56 << 57 << 58; // 1
        internToExtern << 62 << 61; // 2
        internToExtern << 55 << 54 << 53 << 52; // 3
        internToExtern << 51 << 50 << 49 << 46 << 47 << 48; // 4
        internToExtern << 39 << 40 << 41 << 36 << 37 << 38; // 5
        internToExtern << 45 << 44 << 43 << 42; // 6
        internToExtern << 34 << 35 << 32 << 33; // 7
        internToExtern << 31 << 30 << 29 << 26 << 27 << 28; // 8
        internToExtern << 19 << 20 << 21 << 16 << 17 << 18; // 9
        internToExtern << 25 << 24 << 23 << 22; // 10
        internToExtern << 14 << 15 << 12 << 13; // 11
        internToExtern << 11 << 10 <<  9 <<  6 <<  7 <<  8; // 12
        internToExtern <<  1 <<  2 <<  3; // 13
        internToExtern <<  5 <<  4; // 14
        // B3YG
        break;
    case 201:
        size = 33;
        reihe1 = 2;
        reihe2 = 3;
        for (int i =  0; i <  3; i++) { sitzGruppen.append(1); } // 3
        for (int i =  3; i <  9; i++) { sitzGruppen.append(2); } // 6
        for (int i =  9; i < 13; i++) { sitzGruppen.append(3); } // 4
        for (int i = 13; i < 17; i++) { sitzGruppen.append(4); } // 4
        for (int i = 17; i < 23; i++) { sitzGruppen.append(5); } // 6
        for (int i = 23; i < 29; i++) { sitzGruppen.append(6); } // 6
        for (int i = 29; i < 33; i++) { sitzGruppen.append(7); } // 4
        internToExtern <<  3 <<  2 <<  1; // 1
        internToExtern <<  4 <<  5 <<  6 <<  7 <<  8 <<  9; // 2
        internToExtern << 11 << 10 << 12 << 13; // 3
        internToExtern << 21 << 20 << 22 << 23; // 4
        internToExtern << 16 << 15 << 14 << 17 << 18 << 19; // 5
        internToExtern << 24 << 25 << 26 << 27 << 28 << 29; // 6
        internToExtern << 31 << 30 << 32 << 33; // 7
        // BD3YG
        break;
    case 204:
        // WG3yG alt, wie 217 aufgebaut
    case 217:
        size = 40;
        reihe1 = 2;
        reihe2 = 2;
        for (int i =  0; i <  4; i++) { sitzGruppen.append(1); } // 4
        for (int i =  4; i <  8; i++) { sitzGruppen.append(2); } // 4
        for (int i =  8; i < 12; i++) { sitzGruppen.append(3); } // 4
        for (int i = 12; i < 16; i++) { sitzGruppen.append(4); } // 4
        for (int i = 16; i < 20; i++) { sitzGruppen.append(5); } // 4
        for (int i = 20; i < 24; i++) { sitzGruppen.append(6); } // 4
        for (int i = 24; i < 28; i++) { sitzGruppen.append(7); } // 4
        for (int i = 28; i < 32; i++) { sitzGruppen.append(8); } // 4
        for (int i = 32; i < 36; i++) { sitzGruppen.append(9); } // 4
        for (int i = 36; i < 40; i++) { sitzGruppen.append(10); } // 4
        internToExtern <<  6 <<  5 <<  7 <<  8; //  1
        internToExtern <<  2 <<  1 <<  3 <<  4; //  2
        internToExtern << 14 << 13 << 15 << 16; //  3
        internToExtern << 10 <<  9 << 11 << 12; //  4
        internToExtern << 22 << 21 << 23 << 24; //  5
        internToExtern << 18 << 17 << 19 << 20; //  6
        internToExtern << 30 << 29 << 31 << 32; //  7
        internToExtern << 26 << 25 << 27 << 28; //  8
        internToExtern << 38 << 37 << 39 << 40; //  9
        internToExtern << 34 << 33 << 35 << 36; // 10
        // WG3YG neu
        break;

    default:
        size = 0;
        reihe1 = 0;
        reihe2 = 0;
    }

    externToIntern = QHash<int, int>();
    for(int i = 0; i < internToExtern.length(); ++i) {
        externToIntern.insert(internToExtern.at(i), i);
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

int Wagen::klasse()
{
    return klasse(nummer);
}

int Wagen::getAnzahlFreiePlaetzeInSitzgruppe()
{
    if (aktuellePosition == size) return 0; // Out of range
    int gruppe = sitzGruppen.at(aktuellePosition);
    int ende = sitzGruppen.lastIndexOf(gruppe);
    return ende-aktuellePosition+1;
}

void Wagen::verlassePlaetze(QList<int> liste)
{
    // Umrechnen in Interne Darstellung
    if (liste.isEmpty()) return;
    QList<int> intern = extToInt(liste);
    if (intern.isEmpty()) return;
    for(int i = 0; i < intern.length(); i++) {
        verteilung.remove(intern.at(i));
    }
    if (aktuellePosition > intern.first())
        aktuellePosition = intern.first();
}

QList<int> Wagen::besetzePlaetze(Reservierung *r)
{
    QList<int> liste;
    if (r->getAnzahl() > getAnzahlFrei()) return liste;
    int i = 0;
    while (i < r->getAnzahl()) {
        if (! verteilung.contains(aktuellePosition)) {
            liste.append(aktuellePosition);
            verteilung.insert(aktuellePosition, r);
            ++i;
        }
        ++aktuellePosition;
    }
    return intToExt(liste);
}

bool Wagen::besetzePlaetze(Reservierung *r, QList<int> plaetze)
{
    // Umrechnen in interne Darstellung
    QList<int> intern = extToInt(plaetze);
    if (! test(intern, r)) return false;
    for (int pos: qAsConst(intern)) {
        if (pos < 0 || pos > size)
            return false;
        verteilung.insert(pos, r);
        if (pos == aktuellePosition)
            ++aktuellePosition;
    }
    return true;
}

double Wagen::getStrafpunkteFuerPlaetze(int anzahl, int start)
{
    // Vorbereitungen
    start += aktuellePosition;

    int ende = start + anzahl - 1;
    if (ende >= size) return -1; // wenn die Plätze nicht in diesem Wagen platziert werden können

    int sitzGruppeStart = sitzGruppen.at(start);
    int sitzGruppeEnde = sitzGruppen.at(ende);

    int beginnErste = sitzGruppen.indexOf(sitzGruppeStart);
    int endeLetzte = sitzGruppen.lastIndexOf(sitzGruppeEnde);

    // Berechnung der Strafpunkte
    double sp1 = 0;
    double sp2 = 0;

     // Sp 1 //
    double maxB = ceil(anzahl / fmax(2*reihe1, 2*reihe2));

    // Bestimme die belegten Sitzgruppen
    int benoetigt = sitzGruppeEnde - sitzGruppeStart + 1;

    // Sp 2 //
    int ueV = start - beginnErste; // Anzahl der durch fremde Reservierungen belegten Plätze am Anfang
    int ueH = endeLetzte - ende; // Anzahl der durch fremde Reservierungen belegten Sitzplaetze in der letzten Sitzgruppe

    // Zweite Neue Variante zur Berechnung des Fehlers:
    sp1 = (benoetigt - maxB) / maxB;

    if (benoetigt > 1) {
        sp2 = ueV*ueV + ueH*ueH;
    }

    // Gewichtung der einzelnen Strafpunkte //
    return GEWICHT_STRAFE_TEIL_1 * sp1 + GEWICHT_STRAFE_TEIL_2 * sp2;
}

bool Wagen::testPlaetze(QList<int> liste, Reservierung *r)
{
    // Prüft, ob die Plätze frei sind.
    return test(extToInt(liste), r);
}

int Wagen::getAnzahlFrei()
{
    return size-(verteilung.size());
}

int Wagen::getKapazitaet()
{
    return size;
}

bool Wagen::isEmpty()
{
    return verteilung.isEmpty();
}

void Wagen::weisePlaetzeZu()
{
    QHash<Reservierung *, QList<int> > intern; // Plaetze in interner Darstellung
    Reservierung *r;
    for (auto it = verteilung.cbegin(); it != verteilung.cend(); ++it) {
        r = it.value();
        if (!intern.contains(r))
            intern.insert(r, QList<int>());
        QList<int> help = intern.value(r);
        help.append(it.key());
        intern.insert(r, help);
    }

    for (auto it = intern.cbegin(); it != intern.cend(); ++it) {
        QMap<int, QList<int>> plaetze = QMap<int, QList<int>>();
        plaetze.insert(nummer, intToExt(it.value()));
        it.key()->setSitzplatz(plaetze);
    }
}

bool Wagen::test(QList<int> liste, Reservierung *r)
{
    for(int pos: liste) {
        if (pos < size && pos >= 0) {
            if (verteilung.contains(pos) && verteilung.value(pos) != r) {
                return false;
            }
        } else {
            return false;
        }
    }
    return true;
}

QList<int> Wagen::intToExt(QList<int> liste)
{
    QList<int> ext;
    for(int i: liste) {
        ext.append(internToExtern.at(i));
    }
    std::sort(ext.begin(), ext.end());
    return ext;
}

QList<int> Wagen::extToInt(QList<int> liste)
{
    QList<int> intern;
    for(int i: liste) {
        intern.append(externToIntern.value(i));
    }
    std::sort(intern.begin(), intern.end());
    return intern;
}
