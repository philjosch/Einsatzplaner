#ifndef MANAGERRESERVIERUNGEN_H
#define MANAGERRESERVIERUNGEN_H

//#include <QFrame>
#include <QListWidgetItem>
#include "reservierung.h"


class ManagerReservierungen
{
//    Q_OBJECT

public:
    explicit ManagerReservierungen();
    ~ManagerReservierungen();

    QString getWagenreihung() const;
    void setWagenreihung(const QString &value);

    int getAnzahlBelegt();
    int getFrei();

    int getAnzahl();

    QSetIterator<Reservierung *> getReservierungen();

    static QString getStringFromPlaetze(QMap<int, QList<int> *> *liste);
    static QMap<int, QList<int>*> *getPlaetzeFromString(QString plaetze);

public slots:
    void verteileSitzplaetze();
    bool checkPlaetze(QMap<int, QList<int> *> *p);

    Reservierung *createReservierung();
    bool removeReservierung(Reservierung *res);

private:
    bool nehme;



protected:
    QString wagenreihung;
    QSet<Reservierung*> *reservierungen;

};

#endif // MANAGERRESERVIERUNGEN_H
