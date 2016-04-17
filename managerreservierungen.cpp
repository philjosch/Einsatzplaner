#include "managerreservierungen.h"

#include <QJsonArray>
#include <QTime>
#include <QLinkedList>

ManagerReservierungen::ManagerReservierungen(QListWidget *liste)
{
    this->liste = liste;
    map = new QMap<QListWidgetItem*, Reservierung*>();
    reservierungen = new QList<Reservierung*>();
    automatisch = false;
    maximum = -1;
    minimum = 0;
    anzahl = 0;
}

void ManagerReservierungen::releasing()
{
    // Verbindung mit der Liste löschen, d.h. alle Listeneinträge löschen
    while(liste->count() > 0) {
        liste->takeItem(0);
    }
}

void ManagerReservierungen::catching()
{
// Mit QListWidget verbinden
    // D.h. für jede Reservierung ein QListWidgetItem einfügen
    map->clear();
    for (int i = 0; i < reservierungen->size(); i++) {
        QListWidgetItem *item = new QListWidgetItem("");
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsDragEnabled|Qt::ItemIsEnabled);

        reservierungen->at(i)->setListItem(item);
        reservierungen->at(i)->update();
        map->insert(item, reservierungen->at(i));
        liste->insertItem(0, item);
//        liste->insertItem(0, map->keys().at(i));
    }
}

bool ManagerReservierungen::add()
{
    if (anzahl + 1 <= maximum || maximum == -1) {
        QListWidgetItem *item = new QListWidgetItem("");
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsDragEnabled|Qt::ItemIsEnabled);
        Reservierung *res = new Reservierung(item);

        liste->insertItem(0, item);
        liste->setCurrentItem(item);
        reservierungen->append(res);
        map->insert(item, res);
        anzahl ++;
        return true;
    }
    return false;
}

bool ManagerReservierungen::remove(QListWidgetItem *item)
{
    if (anzahl - 1 >= minimum) {
//    QListWidgetItem i = *item;
    // Löschen der jeweiligen Reservierung
        reservierungen->removeAt(reservierungen->indexOf(getReservierung(item)));
        map->remove(item);
        liste->takeItem(liste->row(item));
        anzahl--;
        return true;
    }
    return false;
}

Reservierung *ManagerReservierungen::getReservierung(QListWidgetItem *item)
{
    return map->value(item, new Reservierung(new QListWidgetItem("")));
}

Reservierung *ManagerReservierungen::getCurrentReservierung() {
    return getReservierung(liste->currentItem());
}

int ManagerReservierungen::getGesamtzahl()
{
    int summe = 0;
    for (int i = 0; i < reservierungen->length(); i++) {
        summe += reservierungen->at(i)->getAnzahl();
    }
    return summe;
}

QJsonObject ManagerReservierungen::toJson() {
    QJsonArray a = QJsonArray();
    for(int i = 0; i < reservierungen->length(); i++) {
        a.append(reservierungen->at(i)->toJson());
    }
    QJsonObject o = QJsonObject();
    o.insert("auto", automatisch);
    o.insert("res", a);
    return o;
}

void ManagerReservierungen::fromJson(QJsonObject json) {
//    reservierungen->clear();
    QJsonArray a = json.value("res").toArray();
    for(int i = 0; i < a.size(); i++) {
        Reservierung *res = new Reservierung(NULL);
        res->fromJson(a.at(i).toObject());
        reservierungen->append(res);
    }
    automatisch = json.value("auto").toBool(false);
    minimum = 0; // TODO
    maximum = -1; // TODO
    anzahl = reservierungen->length();
}

void ManagerReservierungen::fromJson(QJsonArray json) {
    reservierungen->clear();
    automatisch = false;
    for(int i = 0; i < json.size(); i++) {
        Reservierung *res = new Reservierung(NULL);
        res->fromJson(json.at(i).toObject());
        reservierungen->append(res);
    }
}

int ManagerReservierungen::getAnzahl() const
{
    return anzahl;
}

int ManagerReservierungen::getMaximum() const
{
    return maximum;
}

bool ManagerReservierungen::setMaximum(int value)
{
    if (value < anzahl) {
        return false;
    }
    maximum = value;
    return true;
}

int ManagerReservierungen::getMinimum() const
{
    return minimum;
}

void ManagerReservierungen::setMinimum(int value)
{
    while (value > anzahl) {
        add();
    }
    minimum = value;
}

bool ManagerReservierungen::getAutomatisch() const
{
    return automatisch;
}

void ManagerReservierungen::setAutomatisch(bool value)
{
    automatisch = value;
}

QList<Reservierung *> *ManagerReservierungen::getReservierungen() const
{
    return reservierungen;
}



void ManagerReservierungen::verteileSitzplaetze(QList<Wagen*> *wagen)
{
    // Hier werden die Sitzplätze verteilt
    count = 0;
    gradeE = 0;
    found = false;
    minimum_grad = 1215752192;
    this->wagen = wagen;
    aktWagen = 0;
    lll = new QSet<int>(); // Erster wert gibt die Sortierung an
    for (int i = 0; i < reservierungen->length(); ++i) {
        lll->insert(i);

/*        int j = i;
        while (j > 0 && ((reservierungen->at(j-1)->getAnzahl() < reservierungen->at(j)->getAnzahl()) || ((reservierungen->at(j-1)->getAnzahl()%4) > (reservierungen->at(j)->getAnzahl() %4)))) {
            int help = lll->value(j-1);
            lll->insert(j-1, lll->value(j));
            lll->insert(j, help);
//            lll->swap(j, j-1);
            --j;
        }
  */
    }
    // Sortieren von l:

/*    int i;
    // Sortieren nach der Größe der Reservierung
    for (int i = 1; i < lll->length(); ++i) {
        int j = i;
        while (j > 0 && ((reservierungen->at(j-1)->getAnzahl() < reservierungen->at(j)->getAnzahl()) || ((reservierungen->at(j-1)->getAnzahl()%4) > (reservierungen->at(j)->getAnzahl() %4)))) {
            lll->swap(j, j-1);
            --j;
        }
    }
  */ /*  for (int i = 1; i < l->length(); ++i) {
        int j = i;
        while (j > 0 && (l->at(j-1)->getAnzahl()%4 > l->at(j)->getAnzahl() %4 || (false))) {
            l->swap(j, j-1);
            --j;
        }
    }
*/
    QTime t;
    t.start();
    verteileSitzplaetze();
    qDebug("Time elapsed: %d ms", t.elapsed());
    qDebug("Counting: %d ", count);
}

void ManagerReservierungen::verteileSitzplaetze()
{
//    count ++;
//    qDebug("%f %f %d", grade(), minimum_grad, lll->length());
    double gg = gradeE;
    if (gg >= minimum_grad) {
//        count += 1;
        return;
    }
    if (lll->isEmpty()) {
        qDebug("Gefunden");
        found = true;
        count += 1;
        for (int i = 0; i < reservierungen->length(); ++i) {
            reservierungen->at(i)->takePlatz();
        }
        minimum_grad = gg;
        return;
    }

    QList<Reservierung *> *lLoc = new QList<Reservierung*>();

    QSet<int> *anzahl = new QSet<int>();

    QList<double> *h = new QList<double>();

    foreach(const int i, *lll) {
        Reservierung *res = reservierungen->at(i);
        if (! anzahl->contains(res->getAnzahl())) {
            anzahl->insert(res->getAnzahl()); // Keine Alternativen verfolgen, die äquivalente Belegungen erzeugen
            if (platziere(res)) {
                int a = gradeYes(res);
                if ((gg+a) <= minimum_grad) {
                    h->append(a);
                    lLoc->append(res);

                    for (int j = h->length()-1; j > 0 &&
                         (h->at(j-1) > h->at(j) ||
                            (h->at(j-1) > h->at(j)*1.5 && lLoc->at(j-1)->getAnzahl() < lLoc->at(j)->getAnzahl())
                          ); j--) {
                        lLoc->swap(j, j-1);
                        h->swap(j, j-1);
                    }
                }
                dePlatziere(res);
            }

        }
    }

    for (int i = 0; i < lLoc->length() && !found; ++i) {
        Reservierung *res = lLoc->at(i);
        if (platziere(res)) {
            double a = gradeYes(res);
            gradeE += a;
            int index = reservierungen->indexOf(res);
            lll->remove(index);
            verteileSitzplaetze();
            gradeE -= a;
            dePlatziere(res);
            lll->insert(index);
        }
    }
}

double ManagerReservierungen::grade()
{
    count ++;
    double summe = 0;
    for (int i = 0; i < reservierungen->length(); ++i) {
        Reservierung *res = reservierungen->at(i);
        if (lll->contains(i)) {
            summe += gradeNo(res);
        } else {
            summe += gradeYes(res);
        }
    }
    return summe;
}
double ManagerReservierungen::gradeYes(Reservierung *res) {
    return res->getWagen()->getStrafpunkte(res->getPlaetze());
}

double ManagerReservierungen::gradeNo(Reservierung *res) {
    // ACHTUNG NUR FÜR 204 und 217 geeignet!!
    double summe = minimum2((4 - ((res->getAnzahl()%10) % 4))% 4, (6 - ((res->getAnzahl()%10) % 6))%6);
    //            summe += (res->getAnzahl() % 4);
    // Konservative Schranke nach unten 2/5 der Plätze wird auf die "falsche" Seite gesetzt
    summe += (double(res->getAnzahl()) * 0.4);
    return summe;
}

int ManagerReservierungen::minimum2(int a, int b) {
    if (a < b) return a;
    return b;
}

bool ManagerReservierungen::platziere(Reservierung *res)
{
    if (wagen->at(aktWagen)->getFreiePlaetze() >= res->getAnzahl()) {
        res->setPlaetze(wagen->at(aktWagen), wagen->at(aktWagen)->besetze(res));
        if (wagen->at(aktWagen)->getFreiePlaetze() == 0) {
            ++aktWagen;
        }
        return true;
    }
    return false;
}

void ManagerReservierungen::dePlatziere(Reservierung *res)
{
    if (aktWagen >= wagen->length() || (wagen->at(aktWagen)->isEmpty() && aktWagen > 0)) {
        --aktWagen;
    }
    wagen->at(aktWagen)->verlasse(res->getPlaetze());
/*    if ((wagen->at(aktWagen)->isEmpty() && aktWagen > 0)) {
        --aktWagen;
    }*/
}

