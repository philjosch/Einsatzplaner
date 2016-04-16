#include "wagen.h"
#include <QSet>

Wagen::Wagen(int nummer)
{
//    this->matrix = matrix;
    aktuellePosition = 0;
    this->nummer = nummer;
    size = 0;
    row1 = 2;

    switch (nummer) {
    case 208:
        size = 60;
        sitzGruppen = QVector<int>(size);
        verteilung = QVector<Reservierung*>(size);
        bewertung = QVector<int>(size);
        row2 = 3;
        for (int i =  0; i <  2; i++) { sitzGruppen[i] =  1; bewertung[i] = 0; } // 2
        for (int i =  2; i <  6; i++) { sitzGruppen[i] =  2; bewertung[i] = 0; } // 4
        for (int i =  6; i < 11; i++) { sitzGruppen[i] =  3; bewertung[i] = 1; } // 5
        for (int i = 11; i < 17; i++) { sitzGruppen[i] =  4; bewertung[i] = 1; } // 6
        for (int i = 17; i < 23; i++) { sitzGruppen[i] =  5; bewertung[i] = 1; } // 6
        for (int i = 23; i < 27; i++) { sitzGruppen[i] =  6; bewertung[i] = 0; } // 4
        for (int i = 27; i < 31; i++) { sitzGruppen[i] =  7; bewertung[i] = 0; } // 4
        for (int i = 31; i < 37; i++) { sitzGruppen[i] =  8; bewertung[i] = 1; } // 6
        for (int i = 37; i < 43; i++) { sitzGruppen[i] =  9; bewertung[i] = 1; } // 6
        for (int i = 43; i < 47; i++) { sitzGruppen[i] = 10; bewertung[i] = 0; } // 4
        for (int i = 47; i < 51; i++) { sitzGruppen[i] = 11; bewertung[i] = 0; } // 4
        for (int i = 51; i < 53; i++) { sitzGruppen[i] = 12; bewertung[i] = 0; } // 2
        for (int i = 53; i < 58; i++) { sitzGruppen[i] = 13; bewertung[i] = 1; } // 5
        for (int i = 58; i < 60; i++) { sitzGruppen[i] = 14; bewertung[i] = 1; } // 2
        // C3YG
        break;
    case 202:
        size = 62;
        sitzGruppen = QVector<int>(size);
        verteilung = QVector<Reservierung*>(size);
        bewertung = QVector<int>(size);
        row2 = 3;
        for (int i =  0; i <  2; i++) { sitzGruppen[i] =  1; bewertung[i] = 0; } // 2
        for (int i =  2; i <  6; i++) { sitzGruppen[i] =  2; bewertung[i] = 0; } // 4
        for (int i =  6; i < 11; i++) { sitzGruppen[i] =  3; bewertung[i] = 1; } // 5
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
/*    case 666:
        size = 16;
        sitzGruppen = QVector<int>(size);
        verteilung = QVector<Reservierung*>(size);
        bewertung = QVector<int>(size);
        for (int i =  0; i <  4; i++) { sitzGruppen[i] =  1; bewertung[i] = 0; } // 4
        for (int i =  4; i <  8; i++) { sitzGruppen[i] =  2; bewertung[i] = 1; } // 4
        for (int i =  8; i < 12; i++) { sitzGruppen[i] =  3; bewertung[i] = 1; } // 4
        for (int i = 12; i < 16; i++) { sitzGruppen[i] =  4; bewertung[i] = 0; } // 4
        //TEST
        break;  */
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
        aktuellePosition--;
    }
}

QList<int> *Wagen::besetze(Reservierung *r)
{
    QList<int> *l = new QList<int>();
    for (int i = 0; i < r->getAnzahl(); ++i) {
        if (verteilung[aktuellePosition] == NULL) {
            verteilung[aktuellePosition] = r;
            l->append(aktuellePosition++);
        } else {
            aktuellePosition++;
            i--;
        }
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

int Wagen::getNummer() const
{
    return nummer;
}

int Wagen::getStrafpunkte(QList<int> *liste)
{
    Reservierung *referenz = verteilung[liste->at(0)];
    int zaehler = 0;
    QSet<int> gruppen = QSet<int>();
    for (int i = 0; i < liste->length(); ++i) {
        gruppen.insert(sitzGruppen.at(liste->at(i)));
    }

    for (int i = 0; i < size; ++i) {
        if ((verteilung[i] != referenz) && (gruppen.contains(sitzGruppen[i]))) {
            ++zaehler;
        }
        if (referenz == verteilung[i]) {
    //        zaehler += bewertung[i];
        }
    }
    return zaehler;
}





