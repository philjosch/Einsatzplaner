#include "managerreservierungen.h"
#include "ui_managerreservierungen.h"

#include <QMessageBox>
#include <QSetIterator>
#include <QSet>

QString ManagerReservierungen::getStringFromPlaetze(QMap<int, QList<int> *> *liste)
{
    QString s = "";
    for(int i: liste->keys()) {
        s += QString::number(i)+": ";
        int old = -1;
        bool strich = false;
        for (int j : * liste->value(i)) {
            if (old+1 == j) {
                if (strich && (j == liste->value(i)->last())) {
                    s += QString::number(j);
                } else if (! strich) {
                    s += "-";
                    strich = true;
                }
            } else {
                if (strich)
                    s += QString::number(old);
                if (j != liste->value(i)->first()) s += ", ";
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

QMap<int, QList<int> *> *ManagerReservierungen::getPlaetzeFromString(QString plaetze)
{
    QMap<int, QList<int>*> *map = new QMap<int, QList<int>*>();
    QStringList l1;
    l1 = plaetze.split(QRegExp("\\s*;\\s*"));
    for (QString s1: l1) {
        QStringList l2a = s1.split(QRegExp("\\s*:\\s*"));
        int wagen = l2a.at(0).toInt();
        if (l2a.length() > 1) {
            QStringList l2 = l2a.at(1).split(QRegExp("\\s*,\\s+"));
            QMessageBox::warning(nullptr, "", "c");
            QList<int> *l = new QList<int>();
            for (QString s2: l2) {
                if (s2.contains(QRegExp("\\s*-\\s*"))) {
                    QStringList l3 = s2.split(QRegExp("\\s*-\\s*"));
                    for (int i = l3.at(0).toInt(); i <= l3.at(1).toInt(); i++) {
                        l->append(i);
                    }
                } else {
                    l->append(s2.toInt());
                }
            }
            map->insert(wagen, l);
        } else {
            map->insert(wagen, new QList<int>());
        }
    }
    return map;
}


ManagerReservierungen::ManagerReservierungen()
{
    wagenreihung = "309, 221, 204, 208";
    reservierungen = new QSet<Reservierung*>();
}

ManagerReservierungen::~ManagerReservierungen()
{

}

QString ManagerReservierungen::getWagenreihung() const
{
    return wagenreihung;
}

void ManagerReservierungen::setWagenreihung(const QString &value)
{
    wagenreihung = value;
}

int ManagerReservierungen::getAnzahlBelegt()
{
    int sum = 0;
    for(Reservierung *r: *reservierungen) {
        sum += r->getAnzahl();
    }
    return sum;
}

int ManagerReservierungen::getFrei()
{
    return -1;
}

int ManagerReservierungen::getAnzahl()
{
    return reservierungen->size();
}

QSetIterator<Reservierung *> ManagerReservierungen::getReservierungen()
{
    QSetIterator<Reservierung*> r(*reservierungen);
    return r;
}


void ManagerReservierungen::verteileSitzplaetze()
{

}

bool ManagerReservierungen::checkPlaetze(QMap<int, QList<int>*> *p)
{
    // Überprüft, ob die eingegebenen Sitzplätze frei sind, oder ob sie schon belegt wu
    return true;
//    return ui->checkFahrrad->isChecked();

}

Reservierung *ManagerReservierungen::createReservierung()
{
    Reservierung *r = new Reservierung();
    reservierungen->insert(r);
    return r;
}

bool ManagerReservierungen::removeReservierung(Reservierung *res)
{
    return reservierungen->remove(res);
}
