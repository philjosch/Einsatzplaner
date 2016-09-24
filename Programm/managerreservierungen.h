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
    ManagerReservierungen(QJsonObject o);
    ~ManagerReservierungen();

    QJsonObject toJson(QJsonObject o);
    QJsonObject toJson();

    QString getWagenreihung() const;
    void setWagenreihung(const QString &value);

    int getAnzahlBelegt();
    int getFrei();

    int getAnzahl();

    QSetIterator<Reservierung *> getReservierungen();

    static QString getStringFromPlaetze(QMap<int, QList<int> *> *liste);
    static QMap<int, QList<int>*> *getPlaetzeFromString(QString plaetze);

    bool getAutoPlatz() const;
    void setAutoPlatz(bool value);

    bool getCheckAll() const;
    void setCheckAll(bool value);

public slots:
    void verteileSitzplaetze();
    bool checkPlaetze(QMap<int, QList<int> *> *p);

    Reservierung *createReservierung();
    bool removeReservierung(Reservierung *res);

protected:
    QString wagenreihung;
    QSet<Reservierung*> *reservierungen;
    bool autoPlatz;
    bool checkAll;

};

#endif // MANAGERRESERVIERUNGEN_H
