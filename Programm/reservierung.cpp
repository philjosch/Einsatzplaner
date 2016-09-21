#include "reservierung.h"

#include "managerreservierungen.h"
#include <QMap>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>

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

Reservierung::Reservierung(QJsonObject o)
{
    name = o.value("name").toString();
    mail = o.value("mail").toString();
    telefon = o.value("telefon").toString();
    anzahl = o.value("anzahl").toInt();
    klasse = o.value("klasse").toInt();
    zuege = new QList<QString>();
    QJsonArray zuegeA = o.value("zuege").toArray();
    for(QJsonValue val: zuegeA) {
        zuege->append(val.toString());
    }
    hps = new QList<QString>();
    QJsonArray hpsA = o.value("hps").toArray();
    for(QJsonValue val: hpsA) {
        hps->append(val.toString());
    }
    sitzplatz = ManagerReservierungen::getPlaetzeFromString(o.value("sitzplaetze").toString());
    fahrrad = o.value("fahrrad").toBool();
    sonstiges = o.value("sonstiges").toString();
}

QJsonObject Reservierung::toJson()
{
    QJsonObject o;
    o.insert("name", name);
    o.insert("mail", mail);
    o.insert("telefon", telefon);
    o.insert("anzahl", anzahl);
    o.insert("klasse", klasse);
    QJsonArray zuegeA;
    for(QString s: *zuege) {
        zuegeA.append(s);
    }
    o.insert("zuege", zuegeA);
    QJsonArray hpsA;
    for(QString s: *hps) {
        hpsA.append(s);
    }
    o.insert("hps", hpsA);
    o.insert("sitzplaetze", ManagerReservierungen::getStringFromPlaetze(sitzplatz));
    o.insert("fahrrad", fahrrad);
    o.insert("sonstiges", sonstiges);
    return o;
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
