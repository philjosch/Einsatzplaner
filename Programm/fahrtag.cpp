#include "fahrtag.h"
#include <QDebug>
#include <QTime>

#include "mainwindow.h"
#include "managerreservierungen.h"

Fahrtag::Fahrtag(QDate date, ManagerPersonal *p): AActivity(date, p), ManagerReservierungen()
{

//    manager = new ManagerReservierungen();

    art = Fahrtag::Museumszug;
    wichtig = false;
    zeitTf = QTime(8, 15);
    zeitAnfang = QTime(8, 45);
    zeitEnde = QTime(18, 45);
    benoetigeTf = true;
    benoetigeZf = true;
    benoetigeZub = true;
    benoetigeService = true;
    personalBenoetigt = false;

    // Listen für Tf, Zf, Zub und Servie müssen noch initalisiert werden
}

Fahrtag::Fahrtag(QJsonObject o, ManagerPersonal *p) : AActivity(o, p), ManagerReservierungen(o)
{
    // Daten für Fahrtag extrahieren
    art = (Art) o.value("art").toInt();
    zeitTf = QTime::fromString(o.value("zeitTf").toString(), "hh:mm");
    wichtig = o.value("wichtig").toBool();
    benoetigeTf = o.value("benoetigeTf").toBool(true);
    benoetigeZf = o.value("benoetigeZf").toBool(true);
    benoetigeZub = o.value("benoetigeZub").toBool(true);
    benoetigeService = o.value("benoetigeService").toBool(true);
}

Fahrtag::~Fahrtag()
{
    AActivity::~AActivity();
}

QJsonObject Fahrtag::toJson()
{
    QJsonObject o = AActivity::toJson();
    o = ManagerReservierungen::toJson(o);
    o.insert("isFahrtag", true);
    o.insert("art", (int) art);
    o.insert("zeitTf", zeitTf.toString("hh:mm"));
    o.insert("wichtig", wichtig);
    o.insert("benoetigeTf", benoetigeTf);
    o.insert("benoetigeZf", benoetigeZf);
    o.insert("benoetigeZub", benoetigeZub);
    o.insert("benoetigeService", benoetigeService);


    return o;
}

QString Fahrtag::getStringFromArt(Fahrtag::Art art)
{
    switch (art) {
    case Museumszug: return "Museumszug";
    case Sonderzug: return "Sonderzug";
    case Gesellschaftssonderzug: return "Gesellschaftssonderzug";
    case Nikolauszug: return "Nikolauszug";
    case Schnupperkurs: return "Ehrenlokführer Schnupperkurs";
    case Bahnhofsfest: return "BAhnhofsfest";
    case ELFundMuseumszug: return "Museumszug mit Schnupperkurs";
    default: return "Sonstiges";
    }
}

QString Fahrtag::getListString()
{
    return datum.toString("dddd dd.MM.yyyy")+" – Fahrtag";
}

QString Fahrtag::getListStringShort() {
    if (anlass.length() == 0) {
        return "Fahrtag";
    }
    return anlass;
}

AActivity::Infos *Fahrtag::getIndividual(Person *person)
{
    Infos *neu = new Infos();
    Infos *alt = personen->value(person);
    neu->aufgabe = alt->aufgabe;
    neu->bemerkung = alt->bemerkung;
    neu->kategorie = alt->kategorie;
    neu->beginn = alt->beginn;
    neu->ende = alt->ende;

    if (alt->beginn == QTime(0,0)) {
        if (alt->kategorie == Tf || alt->kategorie == Tb) {
            neu->beginn = zeitTf;
        } else {
            neu->beginn = zeitAnfang;
        }
    }
    if (alt->ende == QTime(0,0)) {
        neu->ende = zeitEnde;
    }
    return neu;
}

QString Fahrtag::getHtmlForSingleView()
{
    QString html = "";
    // Überschrift
    html += "<h1 class='pb'>" +Fahrtag::getStringFromArt(art) + " am " + datum.toString("dddd dd.MM.yyyy")+(wichtig?" WICHTIG!":"")+"</h1>";
    // Anlass
    if (anlass != "") {
        html += "<p><b>Anlass:</b><br/>"+anlass+"</p>";
    }
    // Wagenreihung
    html += "<p><b>Wagenreihung:</b> "+getWagenreihung()+"</p>";
    // Deisntzeiten
    html += "<p><b>Dienstzeiten</b>:<br/>Beginn Tf, Tb: "+zeitTf.toString("hh:mm")+"<br/>Beginn Sonstige: "+zeitAnfang.toString("hh:mm")+"<br/>";
    html += "Ungefähres Dienstende: "+zeitEnde.toString("hh:mm")+"</p>";
    // Personal
    // *Tf, Tb

    QMap<Person*, AActivity::Infos*> tf;
    QMap<Person*, AActivity::Infos*> zf;
    QMap<Person*, AActivity::Infos*> zub;
    QMap<Person*, AActivity::Infos*> begl;
    QMap<Person*, AActivity::Infos*> service;
    QMap<Person*, AActivity::Infos*> sonstige;

    // Aufsplitten der Personen auf die Einzelnen Listen
    for(Person *p: personen->keys()) {
        switch (personen->value(p)->kategorie) {
        case AActivity::Tf:
        case AActivity::Tb: tf.insert(p, personen->value(p)); break;
        case AActivity::Zf: zf.insert(p, personen->value(p)); break;
        case AActivity::Zub: zub.insert(p, personen->value(p)); break;
        case AActivity::Begleiter: begl.insert(p, personen->value(p)); break;
        case AActivity::Service: service.insert(p, personen->value(p)); break;
        default: sonstige.insert(p, personen->value(p)); break;
        }
    }

    html += "<p><b>Triebfahrzeugführer (Tf), Triebfahrzeugbegleiter(Tb)";
    html += (benoetigeTf ?" werden benötigt": "");
    html += ":</b><br/>"+listToString(&tf, " | ")+"</p>";
    if (art != Fahrtag::Schnupperkurs) {
        // *Zf
        html += "<p><b>Zugführer";
        html += (benoetigeZf ?" wird benötigt":"");
        html += ":</b><br/>"+listToString(&zf, " | ")+"</p>";

        // *Zub, Begl.o.b.A
        html += "<p><b>Zugbegleiter und <i>Begleiter ohne betriebliche Ausbildung</i>";
        html += (benoetigeZub ? " werden benötigt":"");
        html += ":</b><br/>";
        html += listToString(&zub, " | ");
        // Begl. o.b.A
        if (! zub.isEmpty() && ! begl.isEmpty())
            html += " | ";
        html += "<i>"+listToString(&begl, " | ") + "</i></p>";

        // *Service
        html += "<p><b>Service-Personal";
        html += (benoetigeService ?" wird benötigt":"");
        html += ":</b><br/>"+listToString(&service, " | ") +"</p>";
    }
    // *Sonstiges personal
    if (sonstige.size() > 0) {
        html += "<p><b>Sonstiges Personal";
        html += (personalBenoetigt ?" wird benötigt":"");
        html += ":</b><br/>"+listToString(&sonstige, " | ") +"</p>";
    }
    if (bemerkungen != "") {
        html += "<p>Bemerkungen:<br/>"+bemerkungen+"</p>";
    }

    // Reservierungen
    if (getAnzahl() > 0) {
        html += "<p><b>Reservierungen:</b>";
        if (art != Fahrtag::Schnupperkurs && art != Fahrtag::Gesellschaftssonderzug) {
            html += "<br/>Bereits "+QString::number(getBelegtGesamt());
            html += (getBelegtGesamt() == 1 ? " Platz " :  " Plätze ");
            html += "belegt. Noch "+QString::number(getCapacityGesamt() - getBelegtGesamt())+" frei.</p>";
        } else {
            html += "</p>";
        }
        html += "<table cellspacing='0' width='100%'><thead><tr><th>Kontakt</th><th>Sitzplätze</th><th>Ein- und Ausstieg</th><th>Sonstiges</th></tr></thead><tbody>";
        for(Reservierung *r: *reservierungen) {
            html += r->getTableRow();
        }
        html += "</tbody></table>";
    }
    return html;
}

QString Fahrtag::getHtmlForTableView()
{
    QString html = "<tr bgcolor='"+MainWindow::getFarbeZug(art)+"'>";
    // Datum, Anlass
    if (wichtig) {
        html += "<td bgcolor='#ff8888'>";
    } else {
        html += "<td>";
    }
    html += "<b>"+datum.toString("dddd d.M.yyyy")+"</b><br/>("+Fahrtag::getStringFromArt(art)+")";
    if (anlass != "") {
        html += "<br/>"+anlass;
    }
    // Wagenreihung
    if (wagenreihung != "") {
        html += "<br/>"+ wagenreihung;
    }
    html += "</td>";

    QMap<Person*, AActivity::Infos*> tf;
    QMap<Person*, AActivity::Infos*> zf;
    QMap<Person*, AActivity::Infos*> zub;
    QMap<Person*, AActivity::Infos*> begl;
    QMap<Person*, AActivity::Infos*> service;
    QMap<Person*, AActivity::Infos*> sonstige;

    // Aufsplitten der Personen auf die Einzelnen Listen
    for(Person *p: personen->keys()) {
        switch (personen->value(p)->kategorie) {
        case AActivity::Tf:
        case AActivity::Tb: tf.insert(p, personen->value(p)); break;
        case AActivity::Zf: zf.insert(p, personen->value(p)); break;
        case AActivity::Zub: zub.insert(p, personen->value(p)); break;
        case AActivity::Begleiter: begl.insert(p, personen->value(p)); break;
        case AActivity::Service: service.insert(p, personen->value(p)); break;
        default: sonstige.insert(p, personen->value(p)); break;
        }
    }

    // Tf, Tb
    html += "<td>";
    if (benoetigeTf) {
        html += "<b>Lokführer werden benötigt!</b>";
    }
    if (tf.size() > 0) {
        html += "<ul><li>" + listToString(&tf, "</li><li>") + "</li></ul>";
    }
    html += "</td>";

    if (art != Schnupperkurs) {
        // Zf, Zub, Begl.o.b.A.
        html += "<td>";
        if (benoetigeZf) {
            html += "<b><u>Zugführer wird benötigt!</u></b><br/>";
        }
        if (benoetigeZub) {
            html += "<b><i>Zugbegleiter wird benötigt!</i></b>";
        }
        html += "<ul>";
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
        if (benoetigeService) {
            html += "<b>Service-Personal wird benötigt!</b><br/>";
        }
        if (service.size() > 0) {
            html += "<ul><li>" + listToString(&service, "</li><li>") + "</li></ul>";
        }
        html += "</td>";
    } else {
        html += "<td></td><td></td>";
    }

    // Dienstzeiten
    html += "<td>Beginn Tf, Tb: "+zeitTf.toString("hh:mm") + "<br/>";
    if (art != Schnupperkurs) {
        html += "Sonstige: "+zeitAnfang.toString("hh:mm") + "<br/>";
    }
    html += "Ende: ~"+zeitEnde.toString("hh:mm") + "</td>";

    // Sonstiges
    html += "<td>";
    if (personalBenoetigt) {
        html += "<b>Sonstiges Personal wird benötigt!</b><br/>";
    }
    if (sonstige.size() > 0) {
        html += "<ul><li>";
        html += listToString(&sonstige, "</li><li>");
        html += "</li></ul>";
    }
    // Sneek-Peek Reservierungen
    if (art != Schnupperkurs && art != Gesellschaftssonderzug && getBelegtGesamt() > 0) {
        html += QString::number(getBelegtGesamt());
        html += (getBelegtGesamt() == 1 ? " reservierter Sitzplatz": " reservierte Sitzplätze");
        html += " bei " + QString::number(getAnzahl()) + (getAnzahl() == 1 ? " Reservierung" : " Reservierungen");
        html += "<br/>";
    }

    // Bemerkungen
    if (bemerkungen != "") {
        html += bemerkungen;
    }

    html += "</td>";
    html += "</tr>";
    return html;
}

void Fahrtag::emitter()
{
    emit changed(this);
}

bool Fahrtag::getBenoetigeService() const
{
    return benoetigeService;
}

void Fahrtag::setBenoetigeService(bool value)
{
    benoetigeService = value;
    emit changed(this);
}

bool Fahrtag::getBenoetigeZub() const
{
    return benoetigeZub;
}

void Fahrtag::setBenoetigeZub(bool value)
{
    benoetigeZub = value;
    emit changed(this);
}

bool Fahrtag::getBenoetigeZf() const
{
    return benoetigeZf;
}

void Fahrtag::setBenoetigeZf(bool value)
{
    benoetigeZf = value;
    emit changed(this);
}

bool Fahrtag::getBenoetigeTf() const
{
    return benoetigeTf;
}

void Fahrtag::setBenoetigeTf(bool value)
{
    benoetigeTf = value;
    emit changed(this);
}

bool Fahrtag::getWichtig() const
{
    return wichtig;
}

void Fahrtag::setWichtig(bool value)
{
    wichtig = value;
    emit changed(this);
}

QTime Fahrtag::getZeitTf()
{
    return zeitTf;
}

void Fahrtag::setZeitTf(QTime value)
{
    zeitTf = value;
    emit changed(this);
}

Fahrtag::Art Fahrtag::getArt() const
{
    return art;
}

void Fahrtag::setArt(const Fahrtag::Art &value)
{
    art = value;
    emit changed(this);
}
