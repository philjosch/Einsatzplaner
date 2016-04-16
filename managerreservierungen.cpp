#include "managerreservierungen.h"

#include <QJsonArray>

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

void ManagerReservierungen::verteileSitzplaetze(QList<Wagen*> *wagen)
{
    // Hier werden die Sitzplätze verteilt
    minimum_grad = 1215752192;
    this->wagen = wagen;
    aktWagen = 0;
    QList<Reservierung*> *l = new QList<Reservierung*>();
    for (int i = 0; i < reservierungen->length(); ++i) {
        l->append(reservierungen->at(i));
    }
    verteileSitzplaetze(l);
}

void ManagerReservierungen::verteileSitzplaetze(QList<Reservierung *> *liste)
{
    if (liste->isEmpty() && grade(liste) < minimum_grad) {
        for (int i = 0; i < reservierungen->length(); ++i) {
            reservierungen->at(i)->takePlatz();
        }
        minimum_grad = grade(liste);
    }
    if (grade(liste) >= minimum_grad) {
        return;
    }
    for (int i = 0; i < liste->length(); ++i) {
        if (platziere(liste->at(i))) {
            Reservierung *res = liste->at(i);
            liste->removeAt(i);
            verteileSitzplaetze(liste);
            dePlatziere(res);
            liste->insert(i, res);
        }
    }

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

int ManagerReservierungen::grade(QList<Reservierung *> *liste)
{
    int summe = 0;
    for (int i = 0; i < reservierungen->length(); ++i) {
        Reservierung *res = reservierungen->at(i);
        if (liste->contains(res)) {
//            summe += ((res->getAnzahl()-8)%10)*6;
            summe += 0;
        } else {
            summe += res->getWagen()->getStrafpunkte(res->getPlaetze());
        }
    }
    return summe;
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
    if (wagen->at(aktWagen)->isEmpty() && aktWagen > 0) {
        --aktWagen;
    }
    wagen->at(aktWagen)->verlasse(res->getPlaetze());
    if (wagen->at(aktWagen)->isEmpty() && aktWagen > 0) {
        --aktWagen;
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

