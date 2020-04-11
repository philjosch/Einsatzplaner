#include "manager.h"

#include <QJsonArray>
#include <QListIterator>

Manager::Manager()
{
    activities = QList<AActivity*>();
}

QJsonObject Manager::toJson()
{
    QJsonObject data;
    QJsonArray array;
    // Fahrtage und Arbeitseinsätze speichern
    for (int i = 0; i < activities.length(); i++) {
        AActivity *a = activities.at(i);
        array.append(a->toJson());
    }
    data.insert("activites", array);

    // Personen speichern
    QJsonObject personalJSON = personal->toJson();
    data.insert("personal", personalJSON);
    return data;
}

QJsonObject Manager::personalToJson()
{
    QJsonObject data;
    QJsonArray array;
    data.insert("activites", array);

    // Personen speichern
    QJsonObject personalJSON = personal->personalToJson();
    data.insert("personal", personalJSON);
    return data;
}

void Manager::fromJson(QJsonObject c, QJsonObject p)
{
    personal->fromJson(p);

    // Laden der Daten aus dem JSON Object
    activities = QList<AActivity*>();
    QJsonArray array = c.value("activites").toArray();
    for(int i = 0; i < array.size(); i++) {
        QJsonObject aO = array.at(i).toObject();
        AActivity *akt;
        if (aO.value("isFahrtag").toBool()) {
            akt = new Fahrtag(aO, personal);
        } else {
            akt = new Activity(aO, personal);
        }
        activities.append(akt);
        update(activities.length()-1);
    }
}

Fahrtag *Manager::newFahrtag(QDate datum)
{
    Fahrtag *f = new Fahrtag(datum, personal);
    activities.append(f);
    update(activities.length()-1);
    return f;
}

Activity *Manager::newActivity(QDate datum)
{
    Activity *a = new Activity(datum, personal);
    activities.append(a);
    update(activities.length()-1);
    return a;
}

bool Manager::removeActivity(AActivity *a)
{
    a->remove();
    return activities.removeOne(a);
}

void Manager::addActivity(AActivity *a)
{
    activities.append(a);
    update(activities.length()-1);
}

void Manager::activityChanged(AActivity *a)
{
    int pos = activities.indexOf(a);
    if (pos >= 0)
        update(pos);
}

QListIterator<AActivity *> Manager::getActivities() const
{
    QListIterator<AActivity*> i(activities);
    return i;
}

QString Manager::getHtmlFuerListenansicht(QList<AActivity *> liste)
{
    QString a = "<h3>Übersicht über die Aktivitäten</h3>"
                "<table cellspacing='0' width='100%'><thead><tr>"
                "<th>Datum, Anlass</th> <th>Tf, Tb</th> <th><u>Zf</u>, Zub, <i>Begl.o.b.A</i></th>"
                "<th>Service</th> <th>Dienstzeiten</th> <th>Sonstiges</th></tr></thead><tbody>";
    for(AActivity *akt: liste) {
        a += akt->getHtmlForTableView();
    }
    a += "</tbody></table>";
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

void Manager::update(int pos)
{
    // Verschieb das Objekt an Stelle pos an die Richtige Stelle, sodass später schneller darauf zugegriffen werden kann
    int i = pos;
    while (i < activities.length()-1) {
        if (*activities.at(i) > *activities.at(i+1)) {
//        if (activities->at(i)->getDatum() >  activities->at(i+1)->getDatum()) {
            activities.swapItemsAt(i, i+1);
            i++;
        } else {
            break;
        }
    }
    while (i > 0) {
        if (*activities.at(i) <  *activities.at(i-1)) {
            activities.swapItemsAt(i, i-1);
            i--;
        } else {
            break;
        }
    }
}

ManagerPersonal *Manager::getPersonal() const
{
    return personal;
}

void Manager::setPersonal(ManagerPersonal *value)
{
    personal = value;
}
