#ifndef RESERVIERUNG_H
#define RESERVIERUNG_H

#include <QObject>
#include <QList>

class Reservierung
{
public:
    Reservierung();



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
    void setSitzplatz(const QString &value);
    void setSonstiges(const QString &value);

private:
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
    QString sitzplatz;
    QString sonstiges;

};

#endif // RESERVIERUNG_H
