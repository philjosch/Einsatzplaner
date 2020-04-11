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

QString Activity::getKurzbeschreibung()
{
    if (anlass != "")
        return anlass;
    return "Arbeitseinsatz";
}

QString Activity::getListStringShort() {
    if (anlass != "")
        return anlass;
    if (bemerkungen != "")
        return bemerkungen;
    return "Arbeitseinsatz";
}

QString Activity::getListString()
{
    QString scnd = tr("Arbeitseinsatz");
    if (anlass != "")
        scnd = anlass;
    return datum.toString("dddd dd.MM.yyyy")+" – "+scnd;
}

Infos Activity::getIndividual(Person *person)
{
    if (person == nullptr) return Infos();
    if (!personen.contains(person)) return Infos();
    Infos alt = personen.value(person);
    Infos neu = Infos();
    neu.kategorie = alt.kategorie;
    neu.beginn = alt.beginn;
    neu.ende = alt.ende;

    if (zeitenUnbekannt) {
        neu.beginn = QTime(0,0);
        neu.ende = QTime(0,0);
    } else {
        if (alt.beginn == QTime(0,0)) {
            neu.beginn = QTime(zeitAnfang.hour(), zeitAnfang.minute());
        }
        if (alt.ende == QTime(0,0)) {
            neu.ende = QTime(zeitEnde.hour(), zeitEnde.minute());
        }
    }
    return neu;
}

QString Activity::getHtmlForSingleView()
{
    QString required1 = "<font color='"+COLOR_REQUIRED+"'>";
    QString required2 = "</font>";
    QString html = "";
    // Überschrift
    html += "<h2 class='pb'>"+(anlass != "" ? anlass : "Arbeitseinsatz")+" am " + datum.toString("dddd dd.MM.yyyy")+"</h2>";
    // Ort
    if (ort != "")
        html += "<p><b>Ort:</b> "+ort+"</p>";
    // Anlass
    if (anlass != "")
        html += "<p><b>Anlass Arbeitseinsatz:</b><br/>"+anlass+"</p>";
    if (bemerkungen!= "")
        html += "<p><b>Bemerkungen:</b><br/>"+bemerkungen+"</p>";
    // Zeiten
    if (zeitenUnbekannt) {
        html += "<p><b>Zeiten werden noch bekannt gegeben!</b></p>";
    } else {
        html += "<p><b>Zeiten</b>:<br/>Beginn: "+zeitAnfang.toString("hh:mm")+"<br/>";
        if (datum < QDate::currentDate()) {
            html += "Ende: "+zeitEnde.toString("hh:mm")+"</p>";
        } else {
            html += "Geplantes Ende: "+zeitEnde.toString("hh:mm")+"</p>";
        }
    }
    // Personal
    html += "<p><b>Helfer";
    html += (personalBenoetigt ? required1+" werden benötigt"+required2:"");
    html += ":</b></p>";
    if (personen.count() > 0) {
        html += "<table cellspacing='0' width='100%'><thead><tr><th>Name</th><th>Beginn*</th><th>Ende*</th><th>Aufgabe</th></tr></thead><tbody>";
        for(Person *p: personen.keys()) {
            Infos info = personen.value(p);
            html += "<tr><td>"+p->getName()+"</td><td>";
            html += (info.beginn == QTime(0,0) ? "" : info.beginn.toString("hh:mm"));
            html += "</td><td>";
            html += (info.ende == QTime(0,0) ? "" : info.ende.toString("hh:mm"));
            html += "</td><td>";
            if (info.kategorie != Category::Sonstiges) {
                html += getLocalizedStringFromCategory(info.kategorie);
                if (info.bemerkung != "") html += "<br/>";
            }
            html += info.bemerkung;
            html +="</td></tr>";
        }
        html += "</tbody></table><p>* Abweichend von obigen Zeiten!</p>";
    }
    return html;
}

QString Activity::getHtmlForTableView()
{
    QString html = "<tr bgcolor='"+FARBE_FAHRTAGE.value(Arbeitseinsatz)+"'>";
    // Datum, Anlass
    html += "<td>";
    html += "<b>"+datum.toString("dddd d.M.yyyy")+"</b><br</>";
    if (anlass != "") {
        html += anlass;
    } else {
        html += "Arbeitseinsatz";
    }
    if (ort != "") {
        html += " | Ort: "+ort;
    }
    html += "</td>";

    QMap<Person*, Infos> tf;
    QMap<Person*, Infos> zf;
    QMap<Person*, Infos> zub;
    QMap<Person*, Infos> begl;
    QMap<Person*, Infos> service;
    QMap<Person*, Infos> sonstige;
    QMap<Person*, Infos> werkstatt;
    QMap<Person*, Infos> zugvorbereitung;
    QMap<Person*, Infos> ausbildung;
    QMap<Person*, Infos> infrastruktur;

    // Aufsplitten der Personen auf die Einzelnen Listen
    for(Person *p: personen.keys()) {
        switch (personen.value(p).kategorie) {
        case Category::Tf:
        case Category::Tb: tf.insert(p, personen.value(p)); break;
        case Category::Zf: zf.insert(p, personen.value(p)); break;
        case Category::Service: service.insert(p, personen.value(p)); break;
        case Category::Zub: zub.insert(p, personen.value(p)); break;
        case Category::Begleiter: begl.insert(p, personen.value(p)); break;
        case Category::Werkstatt: werkstatt.insert(p, personen.value(p)); break;
        case Category::ZugVorbereiten: zugvorbereitung.insert(p, personen.value(p)); break;
        case Category::Ausbildung: ausbildung.insert(p, personen.value(p)); break;
        case Category::Infrastruktur: infrastruktur.insert(p, personen.value(p)); break;
        default: sonstige.insert(p, personen.value(p)); break;
        }
    }

    // Tf, Tb
    html += "<td>";
    if (tf.size() > 0) {
        html += "<ul><li>" + listToString(tf, "</li><li>") + "</li></ul>";
    }
    html += "</td>";

    // Zf, Zub, Begl.o.b.A.
    html += "<td><ul>";
    if (zf.size() > 0) {
        html += "<li><u>" + listToString(zf, "</u></li><li><u>") + "</u></li>";
    }
    if (zub.size() > 0) {
        html += "<li>" + listToString(zub, "</li><li>") + "</li>";
    }
    if (begl.size() > 0) {
        html += "<li><i>" + listToString(begl, "</i></li><li><i>") + "</i></li>";
    }
    html += "</ul></td>";

    // Service
    html += "<td>";
    if (service.size() > 0) {
        html += "<ul><li>" + listToString(service, "</li><li>") + "</li></ul>";
    }
    html += "</td>";

    // Dienstzeiten
    if (zeitenUnbekannt) {
        html += "<td>Zeiten werden noch bekannt gegeben!</td>";
    } else {
        html += "<td>Beginn: "+zeitAnfang.toString("hh:mm") + "<br/>";
        if (datum < QDate::currentDate()) {
            html += "Ende: "+zeitEnde.toString("hh:mm") + "</td>";
        } else {
            html += "Ende: ~"+zeitEnde.toString("hh:mm") + "</td>";
        }
    }

    // Sonstiges
    html += "<td>";
    if (personalBenoetigt) {
        html += "<b>Helfer werden benötigt!</b>";
    }
    if (werkstatt.size() > 2) {
        html += "<b>Werkstatt:</b><ul style='margin-top: 0px; margin-bottom: 0px'><li>";
        html += listToString(werkstatt, "</li><li>");
        html += "</li></ul>";
    } else {
        for(Person *p: werkstatt.keys()) sonstige.insert(p, werkstatt.value(p));
        werkstatt.clear();
    }
    if (ausbildung.size() > 2) {
        html += "<b>Ausbildung:</b><ul style='margin-top: 0px; margin-bottom: 0px'><li>";
        html += listToString(ausbildung, "</li><li>");
        html += "</li></ul>";
    } else {
        for(Person *p: ausbildung.keys()) sonstige.insert(p, ausbildung.value(p));
        ausbildung.clear();
    }
    if (zugvorbereitung.size() > 2) {
        html += "<b>Zugvorbereitung:</b><ul style='margin-top: 0px; margin-bottom: 0px'><li>";
        html += listToString(zugvorbereitung, "</li><li>");
        html += "</li></ul>";
    } else {
        for(Person *p: zugvorbereitung.keys()) sonstige.insert(p, zugvorbereitung.value(p));
        zugvorbereitung.clear();
    }
    if (sonstige.size() > 0) {
        if (werkstatt.size() + ausbildung.size() + zugvorbereitung.size() > 0) {
            html += "<b>Sonstige:</b><ul style='margin-top: 0px'><li>";
        } else {
            html += "<ul><li>";
        }
        html += listToString(sonstige, "</li><li>", true);
        html += "</li></ul>";
    } else if (werkstatt.size() + ausbildung.size() + zugvorbereitung.size() == 0) {
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

void Activity::deletter()
{
    emit del(this);
}
