#include "managerreservierungen.h"

ManagerReservierungen::ManagerReservierungen(QListWidget *liste)
{
    this->liste = liste;
    map = new QMap<QListWidgetItem*, Reservierung*>();
    reservierungen = new QList<Reservierung*>();
}

void ManagerReservierungen::releasing()
{
 // Verbindung mit der Liste löschen, d.h. alle Listeneinträge löschen
}

void ManagerReservierungen::catching()
{
// Mit QListWidget verbinden
    // D.h. für jede Reservierung ein QListWidgetItem einfügen
}

void ManagerReservierungen::add()
{
    Reservierung *res = new Reservierung();
    res->setName("(Neue Reservierung)");
    liste->addItem("(Neue Reservierung)");
    reservierungen->append(res);
    map->insert(liste->findItems("(Neue Reservierung)", Qt::MatchExactly).at(0), res);
}

void ManagerReservierungen::remove(QListWidgetItem item)
{
//    QListWidgetItem i = *item;
    // Löschen der jeweiligen Reservierung
    reservierungen->removeAt(reservierungen->indexOf(getReservierung(item)));
    map->remove(&item);
    liste->removeItemWidget(&item);
}

void ManagerReservierungen::verteileSitzplaetze()
{
 // Hier werden die Sitzplätze vertiel
    // Algorithmus ist noch nciht vorhanden
}

Reservierung *ManagerReservierungen::getReservierung(QListWidgetItem item)
{
    return map->value(&item, new Reservierung());
}

