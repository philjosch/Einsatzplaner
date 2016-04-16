#ifndef MANAGERRESERVIERUNGEN_H
#define MANAGERRESERVIERUNGEN_H

#include <QListWidget>
#include "reservierung.h"
#include <QJsonObject>
#include "wagen.h"

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

    void verteileSitzplaetze(QList<Wagen *> *wagen); // Ein Algorithmus, der die Sitzplätze der einzelnen Reservierungen verteilt. Muss erst noch entwickelt werden
    void verteileSitzplaetze(QList<Reservierung *> *liste);
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

    int maximum;
    int minimum;

    int anzahl;

    int grade(QList<Reservierung *> *liste);
    bool platziere(Reservierung *res);
    void dePlatziere(Reservierung *res);
    int minimum_grad;
    int aktWagen;
    QList<Wagen*> *wagen;

};


#endif // MANAGERRESERVIERUNGEN_H
