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

void ManagerReservierungen::verteileSitzplaetze(QList<Wagen*> *ersteKlasseWagen, QList<Wagen*> *andereKlasseWagen)
{
    // Verteilen der Reservierungen auf die Gruppen 1. und 2./3. Klasse
    QList<Reservierung*> *ersteKlasseReservierung = new QList<Reservierung*>();
    QList<Reservierung*> *andereKlasseReservierung = new QList<Reservierung*>();

    foreach (Reservierung *res, *reservierungen) {
        if (res->isErsteKlasse()) {
            ersteKlasseReservierung->append(res);
        } else {
            andereKlasseReservierung->append(res);
        }
    }

    Verteiler *verteilerErste = new Verteiler(ersteKlasseWagen, ersteKlasseReservierung);
    verteilerErste->verteile();
    Verteiler *verteilerAndere = new Verteiler(andereKlasseWagen, andereKlasseReservierung);
    verteilerAndere->verteile();
}
