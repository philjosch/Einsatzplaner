#include "wagen.h"
#include <QSet>
#include "math.h"

Wagen::Wagen() {
    Wagen(0);
}

Wagen::Wagen(int nummer)
{
//    this->matrix = matrix;
    aktuellePosition = 0;
    this->nummer = nummer;
    size = 0;
    row1 = 2;
    extra = new QList<int>();

    switch (nummer) {
    case 208:
        size = 60;
        sitzGruppen = QVector<int>(size);
        verteilung = QVector<Reservierung*>(size);
        bewertung = QVector<int>(size);
        row2 = 3;
        *extra << 2 << 2 << 5 << 5;
        for (int i =  0; i <  5; i++) { sitzGruppen[i] =  1; bewertung[i] = 0; } // 5
        for (int i =  5; i <  7; i++) { sitzGruppen[i] =  2; bewertung[i] = 0; } // 2
        for (int i =  7; i < 11; i++) { sitzGruppen[i] =  3; bewertung[i] = 1; } // 4
        for (int i = 11; i < 17; i++) { sitzGruppen[i] =  4; bewertung[i] = 1; } // 6
        for (int i = 17; i < 23; i++) { sitzGruppen[i] =  5; bewertung[i] = 1; } // 6
        for (int i = 23; i < 27; i++) { sitzGruppen[i] =  6; bewertung[i] = 0; } // 4
        for (int i = 27; i < 31; i++) { sitzGruppen[i] =  7; bewertung[i] = 0; } // 4
        for (int i = 31; i < 37; i++) { sitzGruppen[i] =  8; bewertung[i] = 1; } // 6
        for (int i = 37; i < 43; i++) { sitzGruppen[i] =  9; bewertung[i] = 1; } // 6
        for (int i = 43; i < 47; i++) { sitzGruppen[i] = 10; bewertung[i] = 0; } // 4
        for (int i = 47; i < 51; i++) { sitzGruppen[i] = 11; bewertung[i] = 0; } // 4
        for (int i = 51; i < 56; i++) { sitzGruppen[i] = 12; bewertung[i] = 0; } // 5
        for (int i = 56; i < 58; i++) { sitzGruppen[i] = 13; bewertung[i] = 1; } // 2
        for (int i = 58; i < 60; i++) { sitzGruppen[i] = 14; bewertung[i] = 1; } // 2
        // C3YG
        break;
    case 202:
        size = 62;
        sitzGruppen = QVector<int>(size);
        verteilung = QVector<Reservierung*>(size);
        bewertung = QVector<int>(size);
        row2 = 3;
        *extra << 2 << 2 << 5 << 5;
        for (int i =  0; i <  5; i++) { sitzGruppen[i] =  1; bewertung[i] = 0; } // 5
        for (int i =  5; i <  7; i++) { sitzGruppen[i] =  2; bewertung[i] = 0; } // 2
        for (int i =  7; i < 11; i++) { sitzGruppen[i] =  3; bewertung[i] = 1; } // 4
        for (int i = 11; i < 17; i++) { sitzGruppen[i] =  4; bewertung[i] = 1; } // 6
        for (int i = 17; i < 23; i++) { sitzGruppen[i] =  5; bewertung[i] = 1; } // 6
        for (int i = 23; i < 27; i++) { sitzGruppen[i] =  6; bewertung[i] = 0; } // 4
        for (int i = 27; i < 31; i++) { sitzGruppen[i] =  7; bewertung[i] = 0; } // 4
        for (int i = 31; i < 37; i++) { sitzGruppen[i] =  8; bewertung[i] = 1; } // 6
        for (int i = 37; i < 43; i++) { sitzGruppen[i] =  9; bewertung[i] = 1; } // 6
        for (int i = 43; i < 47; i++) { sitzGruppen[i] = 10; bewertung[i] = 0; } // 4
        for (int i = 47; i < 51; i++) { sitzGruppen[i] = 11; bewertung[i] = 0; } // 4
        for (int i = 51; i < 55; i++) { sitzGruppen[i] = 12; bewertung[i] = 0; } // 4
        for (int i = 55; i < 60; i++) { sitzGruppen[i] = 13; bewertung[i] = 1; } // 5
        for (int i = 60; i < 62; i++) { sitzGruppen[i] = 14; bewertung[i] = 1; } // 2
        // B3YG
        break;
    case 201:
        size = 33;
        sitzGruppen = QVector<int>(size);
        verteilung = QVector<Reservierung*>(size);
        bewertung = QVector<int>(size);
        row2 = 2;
        *extra << 3;
        for (int i =  0; i <  4; i++) { sitzGruppen[i] = 1; bewertung[i] = 1; } // 4
        for (int i =  4; i < 10; i++) { sitzGruppen[i] = 2; bewertung[i] = 0; } // 6
        for (int i = 10; i < 16; i++) { sitzGruppen[i] = 3; bewertung[i] = 1; } // 6
        for (int i = 16; i < 20; i++) { sitzGruppen[i] = 4; bewertung[i] = 0; } // 4
        for (int i = 20; i < 24; i++) { sitzGruppen[i] = 5; bewertung[i] = 1; } // 4
        for (int i = 24; i < 30; i++) { sitzGruppen[i] = 6; bewertung[i] = 0; } // 6
        for (int i = 30; i < 33; i++) { sitzGruppen[i] = 7; bewertung[i] = 1; } // 3
        // BD3YG
        break;
    case 204:
        size = 40;
        sitzGruppen = QVector<int>(size);
        verteilung = QVector<Reservierung*>(size);
        bewertung = QVector<int>(size);
        row2 = 2;
        for (int i =  0; i <  4; i++) { sitzGruppen[i] =  1; bewertung[i] = 0; } // 4
        for (int i =  4; i <  8; i++) { sitzGruppen[i] =  2; bewertung[i] = 1; } // 4
        for (int i =  8; i < 12; i++) { sitzGruppen[i] =  3; bewertung[i] = 1; } // 4
        for (int i = 12; i < 16; i++) { sitzGruppen[i] =  4; bewertung[i] = 0; } // 4
        for (int i = 16; i < 20; i++) { sitzGruppen[i] =  5; bewertung[i] = 0; } // 4
        for (int i = 20; i < 24; i++) { sitzGruppen[i] =  6; bewertung[i] = 1; } // 4
        for (int i = 24; i < 28; i++) { sitzGruppen[i] =  7; bewertung[i] = 1; } // 4
        for (int i = 28; i < 32; i++) { sitzGruppen[i] =  8; bewertung[i] = 0; } // 4
        for (int i = 32; i < 36; i++) { sitzGruppen[i] =  9; bewertung[i] = 0; } // 4
        for (int i = 36; i < 40; i++) { sitzGruppen[i] = 10; bewertung[i] = 1; } // 4
        // WG3yG alt
        break;
    case 217:
        size = 40;
        sitzGruppen = QVector<int>(size);
        verteilung = QVector<Reservierung*>(size);
        bewertung = QVector<int>(size);
        row2 = 2;
        for (int i =  0; i <  4; i++) { sitzGruppen[i] =  1; bewertung[i] = 0; } // 4
        for (int i =  4; i <  8; i++) { sitzGruppen[i] =  2; bewertung[i] = 1; } // 4
        for (int i =  8; i < 12; i++) { sitzGruppen[i] =  3; bewertung[i] = 0; } // 4
        for (int i = 12; i < 16; i++) { sitzGruppen[i] =  4; bewertung[i] = 1; } // 4
        for (int i = 16; i < 20; i++) { sitzGruppen[i] =  5; bewertung[i] = 0; } // 4
        for (int i = 20; i < 24; i++) { sitzGruppen[i] =  6; bewertung[i] = 1; } // 4
        for (int i = 24; i < 28; i++) { sitzGruppen[i] =  7; bewertung[i] = 0; } // 4
        for (int i = 28; i < 32; i++) { sitzGruppen[i] =  8; bewertung[i] = 1; } // 4
        for (int i = 32; i < 36; i++) { sitzGruppen[i] =  9; bewertung[i] = 0; } // 4
        for (int i = 36; i < 40; i++) { sitzGruppen[i] = 10; bewertung[i] = 1; } // 4
        // WG3YG neu
        break;
    default:
        size = 0;
        sitzGruppen = QVector<int>(size);
        verteilung = QVector<Reservierung*>(size);
        break;
    }

}

void Wagen::verlasse(QList<int> *liste)
{
    for (int i = 0; i < liste->length(); ++i) {
        verteilung[liste->at(i)] = NULL;
//        aktuellePosition--;
        aktuellePosition = liste->first();
    }
}

QList<int> *Wagen::besetze(Reservierung *r)
{
    QList<int> *l = new QList<int>();
    for (int i = 0; i < r->getAnzahl(); ++i) {
//        if (verteilung[aktuellePosition] == NULL) {
            verteilung[aktuellePosition] = r;
            l->append(aktuellePosition++);
  //      } else {
    //        aktuellePosition++;
      //      i--;
        //}
    }
    return l;
}

int Wagen::getFreiePlaetze()
{
    return size-aktuellePosition;
}

bool Wagen::isEmpty()
{
    return aktuellePosition == 0;
}

QList<int> *Wagen::getExtra() const
{
    return extra;
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

int Wagen::getAnzahlBisEnde()
{
    int i = aktuellePosition;
    while (i < sitzGruppen.length() && sitzGruppen.at(i) == sitzGruppen.at(aktuellePosition)) {
        i++;
    }
    return i-aktuellePosition;
}

int Wagen::getNummer() const
{
    return nummer;
}

double Wagen::getStrafpunkte(QList<int> *liste)
{
    Reservierung *referenz = verteilung[liste->at(0)];
    if (referenz->getName() == "EXTRA!-!-!-") return 0;
    double zaehler = 0;
    QSet<int> gruppen = QSet<int>();
    for (int i = 0; i < liste->length(); ++i) {
        gruppen.insert(sitzGruppen.at(liste->at(i)));
    }
    if (gruppen.size() > 1)
        zaehler += ((double)gruppen.size()*(double)gruppen.size() / liste->length());

    for (int i = sitzGruppen.indexOf(sitzGruppen.at(liste->first()));
         i <= sitzGruppen.lastIndexOf(sitzGruppen.at(liste->last()), sitzGruppen.length()-1); ++i) {
        if ((verteilung[i] != referenz) && (gruppen.contains(sitzGruppen[i])) && (verteilung[i] == NULL || verteilung[i]->getName()!="EXTRA!-!-!-")) {
            zaehler += 1;
//            ++zaehler;
        }
        if ((verteilung[i] != referenz) && (gruppen.contains(sitzGruppen[i])) && (verteilung[i] != NULL && verteilung[i]->getName()=="EXTRA!-!-!-")) {
            zaehler += 0;
        }
        if (referenz == verteilung[i]) {
//            zaehler += bewertung[i];
        }
    }
    zaehler = (double)zaehler;
//    zaehler += gruppen.size();
    return zaehler;
}
// KOMBINATION AUF getStrafpunkte UND besetze
