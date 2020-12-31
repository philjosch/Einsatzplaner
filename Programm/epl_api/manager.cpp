#include "manager.h"

#include <QJsonArray>
#include <QListIterator>

Manager::Manager()
{
    activities = QList<AActivity*>();
    personal = new ManagerPersonal();
}

QJsonArray Manager::toJson()
{
    QJsonArray array;
    // Fahrtage und Arbeitseinsätze speichern
    for(AActivity *a: activities) {
        array.append(a->toJson());
    }
    return array;
}

void Manager::fromJson(QJsonArray array)
{
    // Laden der Daten aus dem JSON Object
    activities = QList<AActivity*>();
    for(int i = 0; i < array.size(); i++) {
        QJsonObject aO = array.at(i).toObject();
        AActivity *akt;
        if (! aO.contains("isFahrtag")) {
            if (Art::Arbeitseinsatz == static_cast<Art>(aO.value("art").toInt())) {
                akt = new Activity(aO, personal);
            } else {
                akt = new Fahrtag(aO, personal);
            }
        } else {
            // Fallback für Version < 1.6
            if (aO.value("isFahrtag").toBool()) {
                akt = new Fahrtag(aO, personal);
            } else {
                akt = new Activity(aO, personal);
            }
        }
        activities.append(akt);
    }
    sort();
}

Fahrtag *Manager::newFahrtag(QDate datum)
{
    Fahrtag *f = new Fahrtag(datum, personal);
    activities.append(f);
    sort();
    return f;
}

Activity *Manager::newActivity(QDate datum)
{
    Activity *a = new Activity(datum, personal);
    activities.append(a);
    sort();
    return a;
}

bool Manager::removeActivity(AActivity *a)
{
    bool ret = activities.removeOne(a);
    delete a;
    return ret;
}

void Manager::sort()
{
    AActivity::sort(&activities);
}

QList<AActivity *> Manager::filter(AActivity::Auswahl auswahl)
{
    QList<AActivity *> liste = QList<AActivity*>();
    for (AActivity *a: activities) {
        if (auswahl.check(a))
            liste.append(a);
    }
    return liste;
}

QList<AActivity *> Manager::getActivities()
{
    return activities;
}

QString Manager::getHtmlFuerListenansicht(QList<AActivity *> liste)
{
    QString a = AActivity::KOPF_LISTE_HTML;
    for(AActivity *akt: liste) {
        a += akt->getHtmlForTableView();
    }
    a += AActivity::FUSS_LISTE_HTML;
    return a;
}

QString Manager::getHtmlFuerEinzelansichten(QList<AActivity *> liste)
{
    QString html = "";
    for(AActivity *a: liste) {
        html += a->getHtmlForSingleView();
        if(liste.last() != a)
            html += "<p class='last'><small>Erstellt am: "+QDateTime::currentDateTime().toString("d.M.yyyy H:mm")+"</small></p>";
        else
            html += "<p><small>Erstellt am: "+QDateTime::currentDateTime().toString("d.M.yyyy H:mm")+"</small></p>";
    }
    return html;
}

ManagerPersonal *Manager::getPersonal() const
{
    return personal;
}
