#include "managerreservierungen.h"

#include <QJsonArray>

ManagerReservierungen::ManagerReservierungen(QListWidget *liste)
{
    this->liste = liste;
    map = new QMap<QListWidgetItem*, Reservierung*>();
    reservierungen = new QList<Reservierung*>();
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

void ManagerReservierungen::add()
{
    QListWidgetItem *item = new QListWidgetItem("");
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsDragEnabled|Qt::ItemIsEnabled);
    Reservierung *res = new Reservierung(item);

    liste->insertItem(0, item);
    liste->setCurrentItem(item);
    reservierungen->append(res);
    map->insert(item, res);
}

void ManagerReservierungen::remove(QListWidgetItem *item) {
//    QListWidgetItem i = *item;
    // Löschen der jeweiligen Reservierung
    reservierungen->removeAt(reservierungen->indexOf(getReservierung(item)));
    map->remove(item);
    liste->takeItem(liste->row(item));
}

void ManagerReservierungen::verteileSitzplaetze() {
 // Hier werden die Sitzplätze vertiel
    // Algorithmus ist noch nciht vorhanden
}

Reservierung *ManagerReservierungen::getReservierung(QListWidgetItem *item) {
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


QJsonArray ManagerReservierungen::toJson() {
    QJsonArray o = QJsonArray();
    for(int i = 0; i < reservierungen->length(); i++) {
        o.append(reservierungen->at(i)->toJson());
    }
    return o;
}

void ManagerReservierungen::fromJson(QJsonArray json) {
    for(int i = 0; i < json.size(); i++) {
        Reservierung *res = new Reservierung(NULL);
        res->fromJson(json.at(i).toObject());
        reservierungen->append(res);
    }
}

void ManagerReservierungen::sortName()
{
    // Liste nach Name sortieren
    for(int i = 1; i < reservierungen->length(); i++) {
        while (i > 0 && reservierungen->at(i-1)->getName() > reservierungen->at(i)->getName()) {
            reservierungen->swap(i, i-1);
        }
    }
}

void ManagerReservierungen::sortPlatz()
{
    // Liste nach Sitzplatz sortieren
    // Liste nach Name sortieren
    for(int i = 1; i < reservierungen->length(); i++) {
        while (i > 0 && reservierungen->at(i-1)->getSitzplatz() > reservierungen->at(i)->getSitzplatz()) {
            reservierungen->swap(i, i-1);
        }
    }
}

QList<Reservierung *> *ManagerReservierungen::getReservierungen() const
{
    return reservierungen;
}


