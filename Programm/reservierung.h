#ifndef RESERVIERUNG_H
#define RESERVIERUNG_H

#include <QObject>
#include <QMap>

class Wagen;

class Reservierung
{
public:
    Reservierung(QMap<int, Wagen *> *wagen);
    Reservierung(QJsonObject o, QMap<int, Wagen *> *wagen);

    QJsonObject toJson();

    QString getName() const;
    void setName(const QString &value);

    QString getMail() const;
    void setMail(const QString &value);

    QString getTelefon() const;
    void setTelefon(const QString &value);

    int getAnzahl() const;
    void setAnzahl(int value);

    int getKlasse() const;
    void setKlasse(const int &value);

    QList<QString> getHps() const;
    void setHps(QList<QString> value);

    QList<QString> getZuege() const;
    void setZuege(QList<QString> value);

    QMap<int, QList<int> > getSitzplatz() const;
    void setSitzplatz(QMap<int, QList<int> > value);

    bool getFahrrad() const;
    void setFahrrad(bool value);

    QString getSonstiges() const;
    void setSonstiges(const QString &value);

    QString getTableRow();
    void removePlaetze();

    QString getHtmlForDetailTable();

    bool inZug(int zug);

private:
    QString name;
    QString mail;
    QString telefon;
    int anzahl;
    int klasse; // 1 = 1. Klasse | 0 = 2./3. Klasse
    QList<QString> zuege;
    QList<QString> hps;
    QMap<int, QList<int>> sitzplatz;
    bool fahrrad;
    QString sonstiges;
    QMap<int, Wagen *> *wagen;
};

#endif // RESERVIERUNG_H
