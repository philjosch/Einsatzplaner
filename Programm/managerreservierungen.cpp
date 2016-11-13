#include "managerreservierungen.h"
#include "wagen.h"
#include "verteiler.h"

#include <QMessageBox>
#include <QSetIterator>
#include <QSet>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QTime>

QString ManagerReservierungen::getStringFromPlaetze(QMap<int, QList<int> *> *liste)
{
    QString s = "";
    for(int i: liste->keys()) {
        s += QString::number(i)+": ";
        int old = -1;
        bool strich = false;
        for (int j : * liste->value(i)) {
            if (old+1 == j) {
                if ((strich && (j == liste->value(i)->last()))) {
                    s += QString::number(j);
                } else if (! strich) {
                    s += "-";
                    strich = true;
                    if (j == liste->value(i)->last()) {
                        s += QString::number(j);
                    }
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
    if (plaetze == "") {
        return map;
    }
    QStringList l1;
    l1 = plaetze.split(QRegExp("\\s*;\\s*"));
    for (QString s1: l1) {
        QStringList l2a = s1.split(QRegExp("\\s*:\\s*"));
        int wagen = l2a.at(0).toInt();
        if (l2a.length() > 1) {
            QStringList l2 = l2a.at(1).split(QRegExp("\\s*,\\s+"));
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
    wagen = new QList<Wagen *>;
    nummerToWagen = new QMap<int, Wagen*>();
    wagenreihung = "309, 221, 204, 208";
    createWagen();
    autoPlatz = false;
    checkAll = false;
    reservierungen = new QSet<Reservierung*>();
}

ManagerReservierungen::ManagerReservierungen(QJsonObject o)
{
    wagen = new QList<Wagen*>();
    nummerToWagen = new QMap<int, Wagen*>();
    wagenreihung = o.value("wagenreihung").toString();
    createWagen();
    checkAll = o.value("checkAll").toBool();
    autoPlatz = o.value("autoPlatz").toBool();
    reservierungen = new QSet<Reservierung*>();
    QJsonArray array = o.value("reservierungen").toArray();
    for(int i = 0; i < array.size(); i++) {
        reservierungen->insert(new Reservierung(array.at(i).toObject(), nummerToWagen));
    }
}

ManagerReservierungen::~ManagerReservierungen()
{

}

QJsonObject ManagerReservierungen::toJson(QJsonObject o)
{
    o.insert("wagenreihung", wagenreihung);
    o.insert("autoPlatz", autoPlatz);
    o.insert("checkAll", checkAll);
    QJsonArray array;
    for(Reservierung *r: reservierungen->values()) {
        array.append(r->toJson());
    }
    o.insert("reservierungen", array);
    return o;
}

QJsonObject ManagerReservierungen::toJson()
{
    QJsonObject o;
    return toJson(o);
}

QString ManagerReservierungen::getWagenreihung() const
{
    return wagenreihung;
}

bool ManagerReservierungen::setWagenreihung(const QString &value)
{
    QString old = wagenreihung;
    wagenreihung = value;
    if (createWagen()) {
        return true;
    }
    wagenreihung = old;
    return false;
}

int ManagerReservierungen::getBelegtGesamt()
{
    int summe = 0;
    for(Reservierung *r: *reservierungen) {
        summe += r->getAnzahl();
    }
    return summe;
}
int ManagerReservierungen::getCapacityGesamt()
{
    int summe = 0;
    for(Wagen *w: *wagen) {
        summe += w->getKapazitaet();
    }
    return summe;
}


int ManagerReservierungen::getBelegtErste()
{
    int summe = 0;
    for(Wagen *w: *wagen) {
        if (w->klasse() == 1) {
            summe += w->getAnzahlBelegt();
        }
    }
    return summe;
}
int ManagerReservierungen::getCapacityErste()
{
    int summe = 0;
    for(Wagen *w: *wagen) {
        if (w->klasse() == 1)
            summe += w->getKapazitaet();
    }
    return summe;
}

int ManagerReservierungen::getBelegtZweite()
{
    int summe = 0;
    for(Wagen *w: *wagen) {
        if (w->klasse() == 2) {
            summe += w->getAnzahlBelegt();
        }
    }
    return summe;
}
int ManagerReservierungen::getCapacityZweite()
{
    int summe = 0;
    for(Wagen *w: *wagen) {
        if (w->klasse() == 2)
            summe += w->getKapazitaet();
    }
    return summe;
}

int ManagerReservierungen::getBelegtDritte()
{
    int summe = 0;
    for(Wagen *w: *wagen) {
        if (w->klasse() == 3) {
            summe += w->getAnzahlBelegt();
        }
    }
    return summe;
}
int ManagerReservierungen::getCapacityDritte()
{
    int summe = 0;
    for(Wagen *w: *wagen) {
        if (w->klasse() == 3)
            summe += w->getKapazitaet();
    }
    return summe;
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

bool ManagerReservierungen::verteileSitzplaetze()
{
    // aufteilen der Wagen auf die beiden Gruppen
    QList<Wagen*> *ersteKlasse = new QList<Wagen*>();
    QList<Wagen*> *andereKlasse = new QList<Wagen*>();
    QStringList wagenR = wagenreihung.split(QRegExp("\\s*,\\s*"));
    for(QString s: wagenR) {
        int nummer = s.toInt();
        switch (Wagen::klasse(nummer)) {
        case 1: ersteKlasse->append(new Wagen(nummer)); break;
        case 2:
        case 3: andereKlasse->append(new Wagen(nummer)); break;
        default: break;
        }
    }
    // Aufteilen der Resevierungen auf die beiden gruppen
    QSet<Reservierung*> *resErste = new QSet<Reservierung*>();
    QSet<Reservierung*> *resAndere = new QSet<Reservierung*>();
    for(Reservierung *r: *reservierungen) {
        if (r->getKlasse() == 0)
            resAndere->insert(r);
        else
            resErste->insert(r);
    }
    // Verteilen der Sitzplätze
    bool ok = true;
    // Erste klasse verteilen
    if (resErste->size() > 0 && ersteKlasse->length() > 0) {
        Verteiler *erste = new Verteiler(ersteKlasse, resErste);
        erste->setCheckAll(checkAll);
        ok = ok && erste->verteile();
    } else if (resErste->size() > 0 && ersteKlasse->length() == 0) {
        ok = false;
    }
    // 2./3. Klasse verteilen
    if (resAndere->size() > 0 && andereKlasse->length() > 0) {
        Verteiler *andere = new Verteiler(andereKlasse, resAndere);
        andere->setCheckAll(checkAll);
        ok = ok && andere->verteile();
    } else if (resAndere->size() > 0 && andereKlasse->length() == 0) {
        ok = false;
    }
    // Die Sitzplätze zuweisen, sodass sie in den Wagen erscheinen.
    for(Reservierung *r: reservierungen->values()) {
        r->setSitzplatz(r->getSitzplatz());
    }
    return ok;
}

bool ManagerReservierungen::checkPlaetze(QMap<int, QList<int>*> *p, Reservierung *r)
{
    // Überprüft, ob die eingegebenen Sitzplätze frei sind, oder ob sie schon belegt wurden
    int summe = 0;
    for(int w: p->keys()) {
        if (! nummerToWagen->contains(w)) return false;
        if (r->getKlasse() == 1 && nummerToWagen->value(w)->klasse() != 1) return false;
        if (r->getKlasse() == 0 && (nummerToWagen->value(w)->klasse() != 2 &&
                                    nummerToWagen->value(w)->klasse() != 3)) return false;
        summe += p->value(w)->length();
        // für jeden Wagen prüfen, ob die Plätze belegt sind
        if (! nummerToWagen->value(w)->testPlaetze(p->value(w), r)) return false;
    }
    if (summe != r->getAnzahl()) return false;
    return true;
}

Reservierung *ManagerReservierungen::createReservierung()
{
    Reservierung *r = new Reservierung(nummerToWagen);
    reservierungen->insert(r);
    return r;
}

bool ManagerReservierungen::removeReservierung(Reservierung *res)
{
    res->removePlaetze();
    return reservierungen->remove(res);
}

bool ManagerReservierungen::createWagen()
{
    QHash<int,Wagen*> wagenNummer;
    for(Wagen *w: *wagen) {
        wagenNummer.insert(w->getNummer(), w);
    }

    QList<Wagen*> *wagenNeu = new QList<Wagen*>();
    QStringList wagenSplit = wagenreihung.split(QRegExp("\\s*,\\s*"));
    for(QString s: wagenSplit) {
        int nummer = s.toInt();
        Wagen *w;
        if (wagenNummer.contains(nummer)) {
            w = wagenNummer.value(nummer);
            wagenNummer.remove(nummer);
        } else {
            if (Wagen::klasse(nummer) == 0) continue;
            w = new Wagen(nummer);
        }
        wagenNeu->append(w);
    }
    if (! wagenNummer.isEmpty()) {
        // Prüfe ob die nicht gelöschten Wagen leer sind
        for(int nummer: wagenNummer.keys()) {
            if (! wagenNummer.value(nummer)->isEmpty())
                return false;
        }
        // Wenn test für alle Wagen überstanden ist, können sie gelöscht werden
    }
    wagen = wagenNeu;
    nummerToWagen->clear();
    for(Wagen *w: *wagen) {
        nummerToWagen->insert(w->getNummer(), w);
    }
    return true;
}

bool ManagerReservierungen::getCheckAll() const
{
    return checkAll;
}

void ManagerReservierungen::setCheckAll(bool value)
{
    checkAll = value;
}

bool ManagerReservierungen::getAutoPlatz() const
{
    return autoPlatz;
}

void ManagerReservierungen::setAutoPlatz(bool value)
{
    autoPlatz = value;
}
