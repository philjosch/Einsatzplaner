#include "managerreservierungen.h"

#include <QJsonArray>

ManagerReservierungen::ManagerReservierungen(QListWidget *liste)
{
    this->liste = liste;
    map = new QMap<QListWidgetItem*, Reservierung*>();
    reservierungen = new QList<Reservierung*>();
    automatisch = false;
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


