#include "reservierung.h"
#include "wagen.h"

#include <QJsonObject>
#include <QJsonArray>

QString Reservierung::getStringFromPlaetze(QMap<int, QList<int> > liste)
{
    QString s = "";
    for(auto it = liste.cbegin(); it != liste.cend(); ++it) {
        s += QString::number(it.key())+": ";
        int old = -1;
        bool strich = false;
        for (int j : it.value()) {
            if (old+1 == j) {
                if ((strich && (j == it.value().last()))) {
                    s += QString::number(j);
                } else if (! strich) {
                    s += "-";
                    strich = true;
                    if (j == it.value().last()) {
                        s += QString::number(j);
                    }
                }
            } else {
                if (strich)
                    s += QString::number(old);
                if (j != it.value().first()) s += ", ";
                s += QString::number(j);
                strich = false;
            }
            old = j;

        }
        s += "; ";
    }
    if (s != "") {
        s = s.left(s.size()-2);
    }
    return s;
}

QMap<int, QList<int> > Reservierung::getPlaetzeFromString(QString plaetze)
{
    QMap<int, QList<int>> map = QMap<int, QList<int>>();
    if (plaetze == "") {
        return map;
    }
    QStringList l1;
    l1 = plaetze.split(QRegExp("\\s*;\\s*"));
    for (const QString &s1: qAsConst(l1)) {
        QStringList l2a = s1.split(QRegExp("\\s*:\\s*"));
        int wagen = l2a.at(0).toInt();
        if (l2a.length() > 1) {
            QStringList l2 = l2a.at(1).split(QRegExp("\\s*,\\s*"));
            QList<int> l = *new QList<int>();
            for (const QString &s2: qAsConst(l2)) {
                if (s2.contains(QRegExp("\\s*-\\s*"))) {
                    QStringList l3 = s2.split(QRegExp("\\s*-\\s*"));
                    for (int i = l3.at(0).toInt(); i <= l3.at(1).toInt(); i++) {
                        l.append(i);
                    }
                } else {
                    l.append(s2.toInt());
                }
            }
            map.insert(wagen, l);
        } else {
            map.insert(wagen, QList<int>());
        }
    }
    return map;
}

bool Reservierung::inZug(int zug) const
{
    if (zuege.contains(zug)) return true;
    if (zuege.length() < 2) return false;
    if (zuege.at(0) != 0) {
        if (zuege.at(1) != 0) {
            if (zuege.at(0) < zug && zug < zuege.at(1)) {
                return true;
            }
        }
    }
    if (zuege.length() < 4) return false;
    if (zuege.at(2) != 0) {
        if (zuege.at(3) != 0) {
            if (zuege.at(2) < zug && zug < zuege.at(3)) {
                return true;
            }
        }
    }
    return false;
}

Reservierung::Reservierung(QMap<int, Wagen *> *wagen)
{
    name = "(Name eingeben)";
    mail = "";
    telefon = "";
    anzahl = 1;
    klasse = 0;
    zuege = QList<int>();
    zuege.append(0);
    zuege.append(0);
    hps = QList<QString>();
    hps.append("-");
    hps.append("-");
    this->wagen = wagen;
    sitzplatz = QMap<int, QList<int>>();
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
    zuege = QList<int>();
    QJsonArray zuegeA = o.value("zuege").toArray();
    for(const QJsonValue &val: qAsConst(zuegeA)) {
        if (! val.isString()) {
            zuege.append(val.toInt());
        } else {
            if (val.toString() == "-") {
                zuege.append(0);
            } else {
                QStringList zL = val.toString().split(" ");
                zuege.append(zL.at(0).toInt());
            }
        }
    }
    hps = QList<QString>();
    QJsonArray hpsA = o.value("hps").toArray();
    for(const QJsonValue &val: qAsConst(hpsA)) {
        hps.append(val.toString());
    }
    this->wagen = wagen;
    sitzplatz = QMap<int, QList<int>>();
    setSitzplatz(getPlaetzeFromString(o.value("sitzplaetze").toString()));
    fahrrad = o.value("fahrrad").toBool();
    sonstiges = o.value("sonstiges").toString().replace("<br/>", "\n");
}

Reservierung::~Reservierung()
{
    removePlaetze();
}

QJsonObject Reservierung::toJson() const
{
    QJsonObject o;
    o.insert("name", name);
    o.insert("mail", mail);
    o.insert("telefon", telefon);
    o.insert("anzahl", anzahl);
    o.insert("klasse", klasse);
    QJsonArray zuegeA;
    for(int s: zuege) {
        zuegeA.append(s);
    }
    o.insert("zuege", zuegeA);
    QJsonArray hpsA;
    for(const QString &s: hps) {
        hpsA.append(s);
    }
    o.insert("hps", hpsA);
    o.insert("sitzplaetze", getStringFromPlaetze(sitzplatz));
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
    emit changed();
}

QString Reservierung::getMail() const
{
    return mail;
}
void Reservierung::setMail(const QString &value)
{
    mail = value;
    emit changed();
}

QString Reservierung::getTelefon() const
{
    return telefon;
}
void Reservierung::setTelefon(const QString &value)
{
    telefon = value;
    emit changed();
}

int Reservierung::getAnzahl() const
{
    return anzahl;
}
void Reservierung::setAnzahl(int value)
{
    anzahl = value;
    emit changed();
}

int Reservierung::getKlasse() const
{
    return klasse;
}
void Reservierung::setKlasse(const int &value)
{
    klasse = value;
    emit changed();
}

QList<QString> Reservierung::getHps() const
{
    return hps;
}
void Reservierung::setHps(QList<QString> value)
{
    hps = value;
    emit changed();
}

QList<int> Reservierung::getZuege() const
{
    return zuege;
}
void Reservierung::setZuege(QList<int> value)
{
    zuege = value;
    emit changed();
}

bool Reservierung::getFahrrad() const
{
    return fahrrad;
}
void Reservierung::setFahrrad(bool value)
{
    fahrrad = value;
    emit changed();
}

QString Reservierung::getSonstiges() const
{
    return sonstiges;
}
void Reservierung::setSonstiges(const QString &value)
{
    sonstiges = value;
    emit changed();
}

QString Reservierung::getHtmlForTable() const
{
    QString html = "<tr><td>"+name+"<br/>"+telefon+"<br/>"+mail+"</td>";
    html += "<td>"+QString::number(anzahl)+" Plätze in ";
    html += (klasse==1 ? "1. Klasse" :  "2./3.Klasse");
    html += "<br/>"+getStringFromPlaetze(sitzplatz)+"</td>";
    html += "<td>";
    for(int i = 0; i < zuege.length(); i=i+2) {
        html +=  (zuege.at(i  ) == 0 ? "" : QString::number(zuege.at(i  )))+" "+(hps.at(i  ) == "-" ? "" : hps.at(i  ))
                +"&rarr; "
                +(zuege.at(i+1) == 0 ? "" : QString::number(zuege.at(i+1)))+" "+(hps.at(i+1) == "-" ? "" : hps.at(i+1));
        if (i+2 < zuege.length()) html += " und <br/>";
    }
    html += "</td>";
    html += (fahrrad ? "<td>Fahrrad!<br/>" : "<td>")+sonstiges+"</td></tr>";
    return html;
}

void Reservierung::removePlaetze()
{
    // Die alten Sitzplätze freigeben
    if (wagen == nullptr) return;
    for(auto it = sitzplatz.cbegin(); it != sitzplatz.cend(); ++it) {
        if (wagen->contains(it.key()))
            wagen->value(it.key())->verlassePlaetze(it.value());
    }
    emit changed();
}

QString Reservierung::getHtmlForDetailTable() const
{
    QString html = "<tr><td>"+name;
    if (telefon != "")
        html += "<br/>"+telefon;
    if (mail != "")
        html += "<br/>"+mail;
    html += "</td>";
    html += "<td>"+QString::number(anzahl)+" Plätze</td>";
    html += "<td>"+getStringFromPlaetze(sitzplatz)+"</td>";
    html += (fahrrad ? "<td>Fahrrad!<br/>" : "<td>");
    if (hps.at(0) != "-")
        html += tr("Einstieg in ")+hps.at(0)+"<br/>";
    if (hps.at(1) != hps.at(0) && hps.at(1) != "-")
        html += tr("Ausstieg in ")+hps.at(1)+"<br/>";
    QString sonst = sonstiges;
    html+=sonst.replace("\n", "<br/>")+"</td></tr>";
    return html;
}

QMap<int, QList<int> > Reservierung::getSitzplatz() const
{
    return sitzplatz;
}

void Reservierung::setSitzplatz(QString value)
{
    setSitzplatz(getPlaetzeFromString(value));
}

void Reservierung::setSitzplatz(QMap<int, QList<int> > value)
{
    removePlaetze();
    // die neuen belegen, wenn möglich
    if (wagen == nullptr) return;
    for (auto it = value.cbegin(); it != value.cend(); ++it) {
        if (wagen->contains(it.key()))
            wagen->value(it.key())->besetzePlaetze(this, it.value());
    }
    sitzplatz = value;
    emit changed();
}
