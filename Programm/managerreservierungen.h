#ifndef MANAGERRESERVIERUNGEN_H
#define MANAGERRESERVIERUNGEN_H

#include <QListWidgetItem>
#include "reservierung.h"
#include "wagen.h"


class ManagerReservierungen
{

public:
    explicit ManagerReservierungen();
    ManagerReservierungen(QJsonObject o);
    ~ManagerReservierungen();

    QJsonObject toJson(QJsonObject o);
    QJsonObject toJson();

    QString getWagenreihung() const;
    bool setWagenreihung(const QString &value);

    int getBelegtGesamt();
    int getCapacityGesamt();

    int getBelegtErste();
    int getCapacityErste();

    int getBelegtZweite();
    int getCapacityZweite();

    int getBelegtDritte();
    int getCapacityDritte();

    int getAnzahl();

    QSetIterator<Reservierung *> getReservierungen();

    static QString getStringFromPlaetze(QMap<int, QList<int> *> *liste);
    static QMap<int, QList<int>*> *getPlaetzeFromString(QString plaetze);

    bool getAutoPlatz() const;
    void setAutoPlatz(bool value);

    bool getCheckAll() const;
    void setCheckAll(bool value);

public slots:
    bool verteileSitzplaetze();
    bool checkPlaetze(QMap<int, QList<int> *> *p, Reservierung *r);

    Reservierung *createReservierung();
    bool removeReservierung(Reservierung *res);

protected:
    QString wagenreihung;
    QSet<Reservierung*> *reservierungen;
    bool autoPlatz;
    bool checkAll;

    QList<Wagen *> *wagen;
    QMap<int,Wagen*> *nummerToWagen;

    bool createWagen();

};

#endif // MANAGERRESERVIERUNGEN_H
