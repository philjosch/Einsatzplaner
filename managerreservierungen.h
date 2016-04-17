#ifndef MANAGERRESERVIERUNGEN_H
#define MANAGERRESERVIERUNGEN_H

#include <QListWidget>
#include "reservierung.h"
#include <QJsonObject>
#include "wagen.h"
#include <QForeachContainer>
#include "verteiler.h"

class ManagerReservierungen {

public:
    QList<Reservierung *> *getReservierungen() const;

    void setAutomatisch(bool value);
    bool getAutomatisch() const;

    int getMinimum() const;
    void setMinimum(int value);

    int getMaximum() const;
    bool setMaximum(int value);

    int getAnzahl() const;

public slots:
    ManagerReservierungen(QListWidget *liste);

    void releasing(); // Löscht die Verbindungen mit dem ListView, d.h. ListView wird freigegeben
    void catching(); // Verbindet die Objekte mit der Liste

    bool add(); // Fügt eine neue Reservierung hinzu, dabei wird auch ein neues Objekt in die Liste eingefügt
    bool remove(QListWidgetItem *item); // Fügt eine neue Reservierung hinzu, dabei wird auch ein neues Objekt in die Liste eingefügt

    Reservierung *getReservierung(QListWidgetItem *item);
    Reservierung *getCurrentReservierung();

    int getGesamtzahl();

    QJsonObject toJson();
    void fromJson(QJsonObject json);
    void fromJson(QJsonArray json);

    void verteileSitzplaetze(QList<Wagen*> *ersteKlasseWagen, QList<Wagen*> *andereKlasseWagen);


private:
    QListWidget *liste; // Hier wird die verknüpfte Liste zur Darstellung gespeichert
    QList<Reservierung *> *reservierungen;
    QMap<QListWidgetItem*, Reservierung*> *map; // Hier werden die Reservierungen gespeichert

    bool automatisch;

    int maximum;
    int minimum;

    int anzahl;
};


#endif // MANAGERRESERVIERUNGEN_H
