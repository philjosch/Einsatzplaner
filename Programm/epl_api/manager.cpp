#include "export.h"
#include "manager.h"

#include <QJsonArray>
#include <QListIterator>

Manager::Manager(ManagerPersonal *manPersonal) : QObject()
{
    activities = QList<AActivity*>();
    personal = manPersonal;
    connect(this, &Manager::changed, this, [=]() { AActivity::sort(&activities);});
}

Manager::Manager(ManagerPersonal *manPersonal, QJsonArray array)
{
    activities = QList<AActivity*>();
    personal = manPersonal;

    // Laden der Daten aus dem JSON Object
    activities = QList<AActivity*>();
    for(int i = 0; i < array.size(); i++) {
        QJsonObject aO = array.at(i).toObject();
        AActivity *akt;
        if (! aO.contains("isFahrtag")) {
            if (Art::Arbeitseinsatz == static_cast<Art>(aO.value("art").toInt())) {
                akt = new AActivity(aO, personal);
            } else {
                akt = new Fahrtag(aO, personal);
            }
        } else {
            // Fallback für Version < 1.6
            if (aO.value("isFahrtag").toBool()) {
                akt = new Fahrtag(aO, personal);
            } else {
                akt = new AActivity(aO, personal);
            }
        }
        connect(akt, &AActivity::changed, this, [=]() { emit changed();});
        connect(akt, &AActivity::changed, this, [=](AActivity *a, QDate date) {emit veraenderteAktivitaet(a, date);});
        activities.append(akt);
    }
    AActivity::sort(&activities);
    connect(this, &Manager::changed, this, [=]() { AActivity::sort(&activities);});
}

QJsonArray Manager::toJson() const
{
    QJsonArray array;
    // Fahrtage und Arbeitseinsätze speichern
    for(AActivity *a: activities) {
        array.append(a->toJson());
    }
    return array;
}

Fahrtag *Manager::newFahrtag(QDate datum)
{
    Fahrtag *f = new Fahrtag(datum, personal);
    activities.append(f);
    AActivity::sort(&activities);
    connect(f, &AActivity::changed, this, [=](AActivity *a, QDate date) {emit veraenderteAktivitaet(a, date);});
    connect(f, &AActivity::changed, this, [=]() {emit changed();});
    emit changed();
    return f;
}

AActivity *Manager::newActivity(QDate datum)
{
    AActivity *a = new AActivity(datum, personal);
    activities.append(a);
    AActivity::sort(&activities);
    connect(a, &AActivity::changed, this, [=](AActivity *a, QDate date) {emit veraenderteAktivitaet(a, date);});
    connect(a, &AActivity::changed, this, [=]() {emit changed();});
    emit changed();
    return a;
}

bool Manager::removeActivity(AActivity *a)
{
    bool ret = activities.removeOne(a);
    delete a;
    emit changed();
    return ret;
}

QList<AActivity *> Manager::filter(Auswahl auswahl) const
{
    QList<AActivity *> liste = QList<AActivity*>();
    for (AActivity *a: activities) {
        if (a->check(auswahl))
            liste.append(a);
    }
    return liste;
}

QList<AActivity *> Manager::getActivities() const
{
    return activities;
}

bool Manager::exportActivitiesListAsHtml(QList<AActivity *> liste, QPrinter *printer)
{
    QString a = AActivity::KOPF_LISTE_HTML;
    for(AActivity *akt: liste) {
        a += akt->getHtmlForTableView();
    }
    a += AActivity::FUSS_LISTE_HTML;

    return Export::druckeHtml(a + Export::zeitStempel(), printer);
}

bool Manager::exportActivitiesDetailAsHtml(QList<AActivity *> liste, QPrinter *printer)
{
    QString html = "";
    for(AActivity *a: liste) {
        html += a->getHtmlForSingleView();
        html += Export::zeitStempel(liste.last() != a);
    }
    return Export::druckeHtml(html, printer);
}
