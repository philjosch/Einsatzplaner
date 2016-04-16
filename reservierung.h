#ifndef RESERVIERUNG_H
#define RESERVIERUNG_H

#include <QObject>
#include <QList>
#include <QListWidgetItem>
#include <QJsonObject>

class Reservierung;

#include "wagen.h"


class Reservierung
{
public:
    Reservierung(QListWidgetItem *item);

    void setName(const QString &value);
    void setAnzahl(int value);
    void setKlasse(const QString &value);
    void setFahrrad(bool value);
    void setMail(const QString &value);
    void setTelefon(const QString &value);
    void setStartZug(const QString &value, int pos);
    void setStartHp(const QString &value, int pos);
    void setEndeZug(const QString &value, int pos);
    void setEndeHp(const QString &value, int pos);
//    void setAutoPlatz(bool value);
    void setSitzplatz(const QString &value);
    void setSonstiges(const QString &value);
    void setListItem(QListWidgetItem *value);

    void update();

    QJsonObject toJson();
    void fromJson(QJsonObject json);

    QString getName() const;
    int getAnzahl() const;
    QString getKlasse() const;
    bool getFahrrad() const;
    QString getMail() const;
    QString getTelefon() const;
    QList<QString> getStartZug() const;
    QList<QString> getStartHp() const;
    QList<QString> getEndeZug() const;
    QList<QString> getEndeHp() const;
//    bool getAutoPlatz() const;
    QString getSitzplatz() const;
    QString getSonstiges() const;

    void setPlaetze(Wagen *value1, QList<int> *value2);
    QList<int> *getPlaetze() const;

    Wagen *getWagen() const;

public slots:
    void takePlatz();


private:
    QListWidgetItem *listItem;
    QString name;
    int anzahl;
    QString klasse;
    bool fahrrad;
    QString mail;
    QString telefon;
    QList<QString> startZug;
    QList<QString> startHp;
    QList<QString> endeZug;
    QList<QString> endeHp;
//    bool autoPlatz;
    QString sitzplatz;
    QString sonstiges;

    Wagen *wagen;
    QList<int> *plaetze;

};

#endif // RESERVIERUNG_H
