#ifndef MANAGERRESERVIERUNGEN_H
#define MANAGERRESERVIERUNGEN_H

#include <QListWidget>
#include "reservierung.h"
#include <QJsonObject>

class ManagerReservierungen {

public:
    QList<Reservierung *> *getReservierungen() const;

    void setAutomatisch(bool value);

    bool getAutomatisch() const;

public slots:
    ManagerReservierungen(QListWidget *liste);

    void releasing(); // Löscht die Verbindungen mit dem ListView, d.h. ListView wird freigegeben
    void catching(); // Verbindet die Objekte mit der Liste

    void add(); // Fügt eine neue Reservierung hinzu, dabei wird auch ein neues Objekt in die Liste eingefügt
    void remove(QListWidgetItem *item); // Fügt eine neue Reservierung hinzu, dabei wird auch ein neues Objekt in die Liste eingefügt

    void verteileSitzplaetze(); // Ein Algorithmus, der die Sitzplätze der einzelnen Reservierungen verteilt. Muss erst noch entwickelt werden
    Reservierung *getReservierung(QListWidgetItem *item);
    Reservierung *getCurrentReservierung();

    int getGesamtzahl();

    QJsonObject toJson();
    void fromJson(QJsonObject json);
    void fromJson(QJsonArray json);


private:
    QListWidget *liste; // Hier wird die verknüpfte Liste zur Darstellung gespeichert
    QList<Reservierung *> *reservierungen;
    QMap<QListWidgetItem*, Reservierung*> *map; // Hier werden die Reservierungen gespeichert

    bool automatisch;

};


#endif // MANAGERRESERVIERUNGEN_H
