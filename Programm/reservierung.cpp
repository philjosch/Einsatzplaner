#include "reservierung.h"

#include "managerreservierungen.h"
#include <QMap>
#include <QList>

Reservierung::Reservierung()
{
    name = "(Name eingeben)";
    mail = "";
    telefon = "";
    anzahl = 1;
    klasse = 0;
    zuege = new QList<QString>();
    zuege->append("2202 SOTW-SSWN");
    zuege->append("2203 SSWN-SOTW");
    hps = new QList<QString>();
    hps->append("Ottweiler");
    hps->append("Ottweiler");
    sitzplatz = new QMap<int, QList<int>*>();
    fahrrad = false;
    sonstiges = "";
}

QString Reservierung::getName() const
{
    return name;
}
void Reservierung::setName(const QString &value)
{
    name = value;
}

QString Reservierung::getMail() const
{
    return mail;
}
void Reservierung::setMail(const QString &value)
{
    mail = value;
}

QString Reservierung::getTelefon() const
{
    return telefon;
}
void Reservierung::setTelefon(const QString &value)
{
    telefon = value;
}

int Reservierung::getAnzahl() const
{
    return anzahl;
}
void Reservierung::setAnzahl(int value)
{
    anzahl = value;
}

int Reservierung::getKlasse() const
{
    return klasse;
}
void Reservierung::setKlasse(const int &value)
{
    klasse = value;
}

QList<QString> *Reservierung::getHps() const
{
    return hps;
}
void Reservierung::setHps(QList<QString> *value)
{
    hps = value;
}

QList<QString> *Reservierung::getZuege() const
{
    return zuege;
}
void Reservierung::setZuege(QList<QString> *value)
{
    zuege = value;
}


bool Reservierung::getFahrrad() const
{
    return fahrrad;
}
void Reservierung::setFahrrad(bool value)
{
    fahrrad = value;
}

QString Reservierung::getSonstiges() const
{
    return sonstiges;
}
void Reservierung::setSonstiges(const QString &value)
{
    sonstiges = value;
}

QString Reservierung::getTableRow()
{
    QString html = "<tr><td>"+name+"<br/>"+telefon+"<br/>"+mail+"</td>";

    html += "<td>"+QString::number(anzahl)+" Plätze in ";
    html += (klasse==1 ? "1. Klasse" :  "2./3.Klasse");
    html += "<br/>"+ManagerReservierungen::getStringFromPlaetze(sitzplatz)+"</td>";

    html += "<td>"+zuege->at(0)+" "+hps->at(0)+"<br/>->"+zuege->at(1)+" "+hps->at(1)+"</td>";

    html += (fahrrad ? "<td>Fahrrad!<br/>" : "<td>")+sonstiges+"</td></tr>";
    return html;
}

QMap<int, QList<int> *> *Reservierung::getSitzplatz() const
{
    return sitzplatz;
}

void Reservierung::setSitzplatz(QMap<int, QList<int> *> *value)
{
    sitzplatz = value;
}
