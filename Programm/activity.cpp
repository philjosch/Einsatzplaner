#include "activity.h"
#include <QMap>

Activity::Activity(QDate *date, ManagerPersonal *p): AActivity(date, p)
{
}

Activity::~Activity()
{
    AActivity::~AActivity();
}

QString Activity::getListStringShort() {
    if (anlass == "") {
        return "Aktivität";
    }
    return anlass;
}

QString Activity::getListString()
{
    return datum->toString("dddd dd.MM.yyyy")+" – Aktivität";
}

AActivity::Infos *Activity::getIndividual(Person *person)
{
    Infos *neu = new Infos();
    Infos *alt = personen->value(person);
    neu->aufgabe = alt->aufgabe;
    neu->bemerkung = alt->bemerkung;
    neu->kategorie = alt->kategorie;
    neu->beginn = alt->beginn;
    neu->ende = alt->ende;

    if (alt->beginn == QTime(0,0)) {
        neu->beginn = QTime(zeitAnfang->hour(), zeitAnfang->minute());
    }
    if (alt->ende == QTime(0,0)) {
        neu->ende = QTime(zeitEnde->hour(), zeitEnde->minute());
    }
    return neu;
}

QString Activity::getHtmlForSingleView()
{
    QString html = "";
    // Überschrift
    html += "<h1 class='pb'>Arbeitseinsatz am " + datum->toString("dddd dd.MM.yyyy")+"</h1>";
    // Ort
    if (ort != "")
        html += "<p><b>Ort:</b> "+ort+"</p>";
    // Anlass
    if (anlass != "")
        html += "<p><b>Anlass:</b><br/>"+anlass+"</p>";
    if (bemerkungen!= "")
        html += "<p>Bemerkungen:<br/>"+bemerkungen+"</p>";
    // Zeiten
    html += "<p><b>Zeiten</b>:<br/>Beginn: "+zeitAnfang->toString("hh:mm")+"<br/>";
    html += "Geplantes Ende: "+zeitEnde->toString("hh:mm")+"</p>";
    // Personal
    html += "<p><b>Helfer";
    html += (personalBenoetigt ?" wird benötigt":"");
    html += ":</b></p>";
    if (personen->count() > 0) {
        html += "<table><thead><tr><th>Name</th><th>Beginn*</th><th>Ende*</th><th>Aufgabe</th></tr></thead><tbody>";
        for(Person *p: personen->keys()) {
            Infos *info = personen->value(p);
            html += "<tr><td>"+p->getName()+"</td><td>";
            html += (info->beginn != QTime(0,0) ? info->beginn.toString("hh:mm") : " ");
            html += "</td><td>";
            html += (info->ende != QTime(0,0) ? info->ende.toString("hh:mm") : " ");
            html += "</td><td>"+info->aufgabe+"</td></tr>";
        }
        html += "</tbody></table>";
    }
    return html;
}

void Activity::handleActivity(AActivity *a)
{
    emit activityModified(a);
}

void Activity::handleEmit()
{
    handleActivity(this);
}
