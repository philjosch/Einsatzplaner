#include "reservierung.h"
#include "managerreservierungen.h"
#include "wagen.h"

#include <QMap>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

Reservierung::Reservierung(QMap<int, Wagen *> *wagen)
{
    name = "(Name eingeben)";
    mail = "";
    telefon = "";
    anzahl = 1;
    klasse = 0;
    zuege = new QList<QString>();
    zuege->append("-");
    zuege->append("-");
    hps = new QList<QString>();
    hps->append("-");
    hps->append("-");
    this->wagen = wagen;
    sitzplatz = new QMap<int, QList<int>>();
    fahrrad = false;
    sonstiges = "";
}

Reservierung::Reservierung(QJsonObject o, QMap<int, Wagen *> *wagen)
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
    this->wagen = wagen;
    sitzplatz = new QMap<int, QList<int>>();
    setSitzplatz(ManagerReservierungen::getPlaetzeFromString(o.value("sitzplaetze").toString()));
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
    html += "<td>";
    for(int i = 0; i < zuege->length(); i=i+2) {
        html +=  (zuege->at(i  ) == "-" ? "" : zuege->at(i  ))+" "+(hps->at(i  ) == "-" ? "" : hps->at(i  ))
                +"->"
                +(zuege->at(i+1) == "-" ? "" : zuege->at(i+1))+" "+(hps->at(i+1) == "-" ? "" : hps->at(i+1));
        if (i+2 < zuege->length()) html += " und <br/>";
    }
    html += "</td>";
    html += (fahrrad ? "<td>Fahrrad!<br/>" : "<td>")+sonstiges+"</td></tr>";
    return html;
}

void Reservierung::removePlaetze()
{
    // Die alten Sitzplätze freigeben
    if (wagen == nullptr) return;
    for(int w: sitzplatz->keys()) {
        if (wagen->contains(w))
            wagen->value(w)->verlassePlaetze(sitzplatz->value(w));
    }
}

QString Reservierung::getHtmlForDetailTable()
{
    QString html = "<tr><td>"+name;
    if (telefon != "")
        html += "<br/>"+telefon;
    if (mail != "")
        html += "<br/>"+mail;
    html += "</td>";
    html += "<td>"+QString::number(anzahl)+" Plätze</td>";
    html += "<td>"+ManagerReservierungen::getStringFromPlaetze(sitzplatz)+"</td>";
    html += (fahrrad ? "<td>Fahrrad!<br/>" : "<td>");
    if (hps->at(0) != "-")
        html += QObject::tr("Einstieg in ")+hps->at(0)+"<br/>";
    if (hps->at(1) != hps->at(0) && hps->at(1) != "-")
        html += QObject::tr("Ausstieg in ")+hps->at(1)+"<br/>";
    html+=sonstiges.replace("\n", "<br/>")+"</td></tr>";
    return html;
}

QMap<int, QList<int>> *Reservierung::getSitzplatz() const
{
    return sitzplatz;
}

void Reservierung::setSitzplatz(QMap<int, QList<int>> *value)
{
    removePlaetze();
    // die neuen belegen, wenn möglich
    if (wagen == nullptr) return;
    for(int w: value->keys()) {
        if (wagen->contains(w))
            wagen->value(w)->besetzePlaetze(this, value->value(w));
    }
    sitzplatz = value;
}
