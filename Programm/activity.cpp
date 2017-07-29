#include "activity.h"
#include "mainwindow.h"

#include <QMap>

Activity::Activity(QDate date, ManagerPersonal *p): AActivity(date, p)
{
}

Activity::Activity(QJsonObject o, ManagerPersonal *p) : AActivity(o, p)
{

}

Activity::~Activity()
{
    AActivity::~AActivity();
}

QJsonObject Activity::toJson()
{
    QJsonObject o = AActivity::toJson();
    return o;
}

QString Activity::getListStringShort() {
    if (anlass == "") {
        return "Aktivität";
    }
    return anlass;
}

QString Activity::getListString()
{
    return datum.toString("dddd dd.MM.yyyy")+" – Aktivität";
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
        neu->beginn = QTime(zeitAnfang.hour(), zeitAnfang.minute());
    }
    if (alt->ende == QTime(0,0)) {
        neu->ende = QTime(zeitEnde.hour(), zeitEnde.minute());
    }
    return neu;
}

QString Activity::getHtmlForSingleView()
{
    QString html = "";
    // Überschrift
    html += "<h1 class='pb'>Arbeitseinsatz am " + datum.toString("dddd dd.MM.yyyy")+"</h1>";
    // Ort
    if (ort != "")
        html += "<p><b>Ort:</b> "+ort+"</p>";
    // Anlass
    if (anlass != "")
        html += "<p><b>Anlass:</b><br/>"+anlass+"</p>";
    if (bemerkungen!= "")
        html += "<p><b>Bemerkungen:</b><br/>"+bemerkungen+"</p>";
    // Zeiten
    html += "<p><b>Zeiten</b>:<br/>Beginn: "+zeitAnfang.toString("hh:mm")+"<br/>";
    html += "Geplantes Ende: "+zeitEnde.toString("hh:mm")+"</p>";
    // Personal
    html += "<p><b>Helfer";
    html += (personalBenoetigt ? " werden benötigt":"");
    html += ":</b></p>";
    if (personen->count() > 0) {
        html += "<table cellspacing='0' width='100%'><thead><tr><th>Name</th><th>Beginn*</th><th>Ende*</th><th>Aufgabe</th></tr></thead><tbody>";
        for(Person *p: personen->keys()) {
            Infos *info = personen->value(p);
            html += "<tr><td>"+p->getName()+"</td><td>";
            html += (info->beginn != QTime(0,0) ? info->beginn.toString("hh:mm") : " ");
            html += "</td><td>";
            html += (info->ende != QTime(0,0) ? info->ende.toString("hh:mm") : " ");
            if (info->aufgabe == "")
                html += "</td><td>"+AActivity::getStringFromCategory(info->kategorie)+"</td></tr>";
            else
                html += "</td><td>"+info->aufgabe+"</td></tr>";
        }
        html += "</tbody></table><p>* Abweichend von obigen Zeiten!</p>";
    }
    return html;
}

QString Activity::getHtmlForTableView()
{
    QString html = "<tr bgcolor='"+MainWindow::getFarbeArbeit()+"'>";
    // Datum, Anlass
    html += "<td>";
    html += "<b>"+datum.toString("dddd d.M.yyyy")+"</b><br/>(Arbeitseinsatz)<br/>"+anlass+"</td>";

    QMap<Person*, AActivity::Infos*> tf;
    QMap<Person*, AActivity::Infos*> zf;
    QMap<Person*, AActivity::Infos*> zub;
    QMap<Person*, AActivity::Infos*> begl;
    QMap<Person*, AActivity::Infos*> service;
    QMap<Person*, AActivity::Infos*> sonstige;

    // Aufsplitten der Personen auf die Einzelnen Listen
    for(Person *p: personen->keys()) {
        switch (personen->value(p)->kategorie) {
        case Category::Tf:
        case Category::Tb: tf.insert(p, personen->value(p)); break;
        case Category::Zf: zf.insert(p, personen->value(p)); break;
        case Category::Zub: zub.insert(p, personen->value(p)); break;
        case Category::Begleiter: begl.insert(p, personen->value(p)); break;
        case Category::Service: service.insert(p, personen->value(p)); break;
        default: sonstige.insert(p, personen->value(p)); break;
        }
    }

    // Tf, Tb
    html += "<td>";
    if (tf.size() > 0) {
        html += "<ul><li>" + listToString(&tf, "</li><li>") + "</li></ul>";
    }
    html += "</td>";

    // Zf, Zub, Begl.o.b.A.
    html += "<td><ul>";
    if (zf.size() > 0) {
        html += "<li><u>" + listToString(&zf, "</u></li><li><u>") + "</u></li>";
    }
    if (zub.size() > 0) {
        html += "<li>" + listToString(&zub, "</li><li>") + "</li>";
    }
    if (begl.size() > 0) {
        html += "<li><i>" + listToString(&begl, "</i></li><li><i>") + "</i></li>";
    }
    html += "</ul></td>";

    // Service
    html += "<td>";
    if (service.size() > 0) {
        html += "<ul><li>" + listToString(&service, "</li><li>") + "</li></ul>";
    }
    html += "</td>";

    // Dienstzeiten
    html += "<td>Beginn: "+zeitAnfang.toString("hh:mm") + "<br/>";
    html += "Ende: ~"+zeitEnde.toString("hh:mm") + "</td>";

    // Sonstiges
    html += "<td>";
    if (personalBenoetigt) {
        html += "<b>Helfer werden benötigt!</b>";
    }
    if (sonstige.size() > 0) {
        html += "<ul><li>";
        html += listToString(&sonstige, "</li><li>", true);
        html += "</li></ul>";
    } else {
        html += "<br/>";
    }
    // Bemerkungen
    if (bemerkungen != "") {
        html += bemerkungen;
    }
    html += "</td></tr>";
    return html;
}

void Activity::emitter()
{
    emit changed(this);
}
