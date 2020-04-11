#include "fahrtag.h"
#include "mainwindow.h"
#include "managerreservierungen.h"

Fahrtag::Fahrtag(QDate date, ManagerPersonal *p): AActivity(date, p), ManagerReservierungen()
{
    art = Museumszug;
    wichtig = false;
    zeitTf = QTime(8, 15);
    zeitAnfang = QTime(8, 45);
    zeitEnde = QTime(18, 45);
    benoetigeTf = true;
    benoetigeZf = true;
    benoetigeZub = true;
    benoetigeService = true;
    personalBenoetigt = false;
}

Fahrtag::Fahrtag(QJsonObject o, ManagerPersonal *p) : AActivity(o, p), ManagerReservierungen(o)
{
    // Daten für Fahrtag extrahieren
    art = static_cast<Art>(o.value("art").toInt());
    zeitTf = QTime::fromString(o.value("zeitTf").toString(), "hh:mm");
    wichtig = o.value("wichtig").toBool();
    if (! o.value("benoetigeTf").isBool()) {
        benoetigeTf = o.value("benoetigeTf").toInt(1);
    } else {
        if (o.value("benoetigeTf").toBool(true)) {
            benoetigeTf = 1;
        } else {
            benoetigeTf = 0;
        }
    }
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
    o.insert("art", art);
    o.insert("zeitTf", zeitTf.toString("hh:mm"));
    o.insert("wichtig", wichtig);
    o.insert("benoetigeTf", benoetigeTf);
    o.insert("benoetigeZf", benoetigeZf);
    o.insert("benoetigeZub", benoetigeZub);
    o.insert("benoetigeService", benoetigeService);
    return o;
}

QString Fahrtag::getStringFromArt(Art art)
{
    switch (art) {
    case Museumszug: return "Museumszug";
    case Sonderzug: return "Sonderzug";
    case Gesellschaftssonderzug: return "Gesellschaftssonderzug";
    case Nikolauszug: return "Nikolauszug";
    case Schnupperkurs: return "Ehrenlokführer Schnupperkurs";
    case Bahnhofsfest: return "Bahnhofsfest";
    case ELFundMuseumszug: return "Museumszug mit Schnupperkurs";
    default: return "Sonstiges";
    }
}

QString Fahrtag::getKurzbeschreibung()
{
    return getStringFromArt(art);
}

QString Fahrtag::getListString()
{
    QString s = datum.toString(QObject::tr("dddd dd.MM.yyyy"))+" – "+getStringFromArt(art);
    if (wichtig) {
        s += " WICHTIG!";
    }
    return s;
}

QString Fahrtag::getListStringShort() {
    if (anlass.length() != 0)
        return anlass;
    return getStringFromArt(art);
}

Infos Fahrtag::getIndividual(Person *person)
{
    if (person == nullptr) return Infos();
    if (!personen.contains(person)) return Infos();
    Infos alt = personen.value(person);
    Infos neu = Infos();
    neu.bemerkung = alt.bemerkung;
    neu.kategorie = alt.kategorie;
    neu.beginn = alt.beginn;
    neu.ende = alt.ende;

    if (zeitenUnbekannt) {
        neu.beginn = QTime(0,0);
        neu.ende = QTime(0,0);
    } else {
        if (alt.beginn == QTime(0,0)) {
            if (alt.kategorie == Tf) {
                neu.beginn = zeitTf;
            } else {
                neu.beginn = zeitAnfang;
            }
        }
        if (alt.ende == QTime(0,0)) {
            neu.ende = zeitEnde;
        }
    }
    return neu;
}

QString Fahrtag::getHtmlForSingleView()
{
    QString required1 = "<font color='"+COLOR_REQUIRED+"'>";
    QString required2 = "</font>";
    QString html = "";
    // Überschrift
    html += "<h2 class='pb'>" +Fahrtag::getStringFromArt(art) + " am " + datum.toString("dddd dd.MM.yyyy")+(wichtig?required1+" WICHTIG!"+required2:"")+"</h2>";
    // Anlass
    if (anlass != "") {
        html += "<p><b>Anlass:</b><br/>"+anlass+"</p>";
    }
    // Wagenreihung
    html += "<p><b>Wagenreihung:</b> "+getWagenreihung()+"</p>";
    // Dienstzeiten
    if (zeitenUnbekannt) {
        html += "<p><b>Dienstzeiten werden noch bekannt gegeben!</b></p>";
    } else {
        html += "<p><b>Dienstzeiten</b>:<br/>Beginn Tf: "+zeitTf.toString("hh:mm")+"<br/>Beginn Sonstige: "+zeitAnfang.toString("hh:mm")+"<br/>";
        if (datum < QDate::currentDate()) {
            html += "Ende: "+zeitEnde.toString("hh:mm")+"</p>";
        } else {
            html += "Ungefähres Dienstende: "+zeitEnde.toString("hh:mm")+"</p>";
        }
    }

    // Personal
    QMap<Person*, Infos> tf;
    QMap<Person*, Infos> zf;
    QMap<Person*, Infos> zub;
    QMap<Person*, Infos> begl;
    QMap<Person*, Infos> service;
    QMap<Person*, Infos> sonstige;

    // Aufsplitten der Personen auf die Einzelnen Listen
    for(Person *p: personen.keys()) {
        switch (personen.value(p).kategorie) {
        case Category::Tf:
        case Category::Tb: tf.insert(p, personen.value(p)); break;
        case Category::Zf: zf.insert(p, personen.value(p)); break;
        case Category::Zub: zub.insert(p, personen.value(p)); break;
        case Category::Begleiter: begl.insert(p, personen.value(p)); break;
        case Category::Service: service.insert(p, personen.value(p)); break;
        default: sonstige.insert(p, personen.value(p)); break;
        }
    }
    // *Tf/Tb
    if (benoetigeTf || tf.size() > 0) {
        html += "<p><b>";
        if (benoetigeTf) {
            html += required1+QString::number(benoetigeTf)+required2
                    +" Triebfahrzeugführer (Tf) "
                    +required1+"benötigt"+required2;
        } else {
            html += "Triebfahrzeugführer (Tf)";
        }
        html += ":</b><br/>"+listToString(tf, " | ")+"</p>";
    }
    // *Zf
    if ((benoetigeZf && (art != Schnupperkurs)) || zf.size() > 0) {
        html += "<p><b>Zugführer";
        html += (benoetigeZf && (art != Schnupperkurs) ? required1+" wird benötigt"+required2:"");
        html += ":</b><br/>"+listToString(zf, " | ")+"</p>";
    }
    // *Zub, Begl.o.b.A
    if ((benoetigeZub && (art != Schnupperkurs)) || (zub.size() > 0 || begl.size() > 0)) {
        html += "<p><b>Zugbegleiter und <i>Begleiter ohne betriebliche Ausbildung</i>";
        html += (benoetigeZub && (art != Schnupperkurs) ? required1+" werden benötigt"+required2:"");
        html += ":</b><br/>";
        html += listToString(zub, " | ");
        // Begl. o.b.A
        if (! zub.isEmpty() && ! begl.isEmpty())
            html += " | ";
        html += "<i>"+listToString(begl, "</i> | <i>") + "</i></p>";
    }
    // *Service
    if ((benoetigeService && (art != Schnupperkurs)) || service.size() > 0) {
        html += "<p><b>Service-Personal";
        html += (benoetigeService && (art != Schnupperkurs) ? required1+" wird benötigt"+required2:"");
        html += ":</b><br/>"+listToString(service, " | ") +"</p>";
    }
    // *Sonstiges personal
    if (sonstige.size() > 0) {
        html += "<p><b>Sonstiges Personal";
        html += (personalBenoetigt ? required1+" wird benötigt"+required2:"");
        html += ":</b><br/>"+listToString(sonstige, " | ", true) +"</p>";
    }
    if (bemerkungen != "") {
        html += "<p><b>Bemerkungen:</b><br/>"+bemerkungen.replace("\n", "<br/>")+"</p>";
    }

    // Reservierungen
    if (getAnzahl() > 0) {
        html += "<p><b>Reservierungen:</b>";
        if (art != Schnupperkurs && art != Gesellschaftssonderzug) {
            html += "<br/>Bereits "+QString::number(getBelegtGesamt());
            html += (getBelegtGesamt() == 1 ? " Platz " :  " Plätze ");
            html += "belegt. Noch "+QString::number(getCapacityGesamt() - getBelegtGesamt())+" frei.</p>";
        } else {
            html += "</p>";
        }
        if (art != Nikolauszug) {
            html += "<table cellspacing='0' width='100%'><thead><tr><th>Kontakt</th><th>Sitzplätze</th><th>Ein- und Ausstieg</th><th>Sonstiges</th></tr></thead><tbody>";
            for(Reservierung *r: reservierungen) {
                html += r->getTableRow();
            }
            html += "</tbody></table>";
        }
    }
    return html;
}

QString Fahrtag::getHtmlForTableView()
{
    QString html = "<tr bgcolor='"+FARBE_FAHRTAGE.value(art)+"'>";
    // Datum, Anlass
    if (wichtig) {
        html += "<td bgcolor='#ff8888'>";
    } else {
        html += "<td>";
    }
    html += "<b>"+datum.toString("dddd d.M.yyyy")+"</b><br/>("+Fahrtag::getStringFromArt(art)+")";
    // Wagenreihung
    if (wagenreihung != "") {
        html += "<br/>"+ wagenreihung;
    }
    if (anlass != "") {
        html += "<br/>"+anlass.replace("\n", "<br/>");
    }
    html += "</td>";

    QMap<Person*, Infos> tf;
    QMap<Person*, Infos> zf;
    QMap<Person*, Infos> zub;
    QMap<Person*, Infos> begl;
    QMap<Person*, Infos> service;
    QMap<Person*, Infos> sonstige;

    // Aufsplitten der Personen auf die Einzelnen Listen
    for(Person *p: personen.keys()) {
        switch (personen.value(p).kategorie) {
        case Category::Tf:
        case Category::Tb: tf.insert(p, personen.value(p)); break;
        case Category::Zf: zf.insert(p, personen.value(p)); break;
        case Category::Zub: zub.insert(p, personen.value(p)); break;
        case Category::Begleiter: begl.insert(p, personen.value(p)); break;
        case Category::Service: service.insert(p, personen.value(p)); break;
        default: sonstige.insert(p, personen.value(p)); break;
        }
    }

    // Tf, Tb
    html += "<td>";
    if (benoetigeTf > 0) {
        html += "<b>"+QString::number(benoetigeTf)+" Lokführer benötigt!</b>";
    }
    if (tf.size() > 0) {
        html += "<ul><li>" + listToString(tf, "</li><li>") + "</li></ul>";
    }
    html += "</td>";

    // Zf, Zub, Begl.o.b.A.
    html += "<td>";
    if (benoetigeZf && (art != Schnupperkurs)) {
        html += "<b><u>Zugführer wird benötigt!</u></b><br/>";
    }
    if (benoetigeZub && (art != Schnupperkurs)) {
        html += "<b><i>Zugbegleiter wird benötigt!</i></b>";
    }
    html += "<ul>";
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
    if (benoetigeService && (art != Schnupperkurs)) {
        html += "<b>Service-Personal wird benötigt!</b>";
    }
    if (service.size() > 0) {
        html += "<ul><li>" + listToString(service, "</li><li>") + "</li></ul>";
    }
    html += "</td>";

    // Dienstzeiten
    if (zeitenUnbekannt) {
        html += "<td>Dienstzeiten werden noch bekannt gegeben!</td>";
    } else {
        html += "<td>Beginn Tf: "+zeitTf.toString("hh:mm") + "<br/>";
        if (art != Schnupperkurs) {
            html += "Sonstige: "+zeitAnfang.toString("hh:mm") + "<br/>";
        }
        if (datum < QDate::currentDate()) {
            html += "Ende: "+zeitEnde.toString("hh:mm") + "</td>";
        } else {
            html += "Ende: ~"+zeitEnde.toString("hh:mm") + "</td>";
        }
    }

    // Sonstiges
    html += "<td>";
    if (personalBenoetigt) {
        html += "<b>Sonstiges Personal wird benötigt!</b><br/>";
    }
    if (sonstige.size() > 0) {
        html += "<ul><li>";
        html += listToString(sonstige, "</li><li>", true);
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
        html += bemerkungen.replace("\n", "<br/>");
    }

    html += "</td>";
    html += "</tr>";
    return html;
}

void Fahrtag::emitter()
{
    emit changed(this);
}

void Fahrtag::deletter()
{
    emit del(this);
}

QString Fahrtag::getHtmlFuerReservierungsuebersicht()
{
    QString a = "<h3>";
    a += getStringFromArt(art)+" am "+datum.toString("dddd dd. MM. yyyy");
    a += " - Die Reservierungen</h3>";
    a += "<table cellspacing='0' width='100%'><thead><tr> <th>Name</th> <th>Anzahl</th> <th>Sitzplätze</th> <th>Sonstiges</th></tr></thead><tbody>";
    // Sortieren der Daten nach Wagenreihung
    QStringList wagen = wagenreihung.split(QRegExp("\\s*,\\s*"));
    QList<int> wagenNummern;
    for(QString s: wagen) wagenNummern.append(s.toInt());

    // Sortieren der Reservierungen
    QHash<int, QList<Reservierung*>*> wagenZuRes;
    foreach (Reservierung *r, reservierungen) {
        for(int i: r->getSitzplatz().keys()) {
            if (!wagenZuRes.contains(i))
                wagenZuRes.insert(i, new QList<Reservierung*>());
            int pos = 0;
            QList<Reservierung*> *list= wagenZuRes.value(i);
            list->insert(0, r);
            while (pos+1 < list->length() && list->at(pos)->getName() > list->at(pos+1)->getName()) {
                list->swapItemsAt(pos, pos+1);
                pos++;
            }
        }
        if (r->getSitzplatz().isEmpty()) {
            if (! wagenZuRes.contains(999))
                wagenZuRes.insert(999, new QList<Reservierung*>());
            int pos = 0;
            QList<Reservierung*> *list= wagenZuRes.value(999);
            list->insert(0, r);
            while (pos+1 < list->length() && list->at(pos)->getName() > list->at(pos+1)->getName()) {
                list->swapItemsAt(pos, pos+1);
                pos++;
            }
        }
    }

    for(int wagenNr: wagenNummern) {
        if (wagenZuRes.contains(wagenNr)) {
            a += "<tr><td columnspan='4'><b>Wagen "+QString::number(wagenNr)+":</b></td></tr>";
            for(Reservierung *r: *wagenZuRes.value(wagenNr)) {
                a += r->getHtmlForDetailTable();
            }
        }
    }
    if (wagenZuRes.contains(999)) {
        a += "<tr><td columnspan='4'><b>Reservierungen ohne Sitzplätze:</b></td></tr>";
        for(Reservierung *r: *wagenZuRes.value(999)) {
            a += r->getHtmlForDetailTable();
        }
    }

    a += "</tbody></table>";
    return a;
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

int Fahrtag::getBenoetigeTf() const
{
    return benoetigeTf;
}

void Fahrtag::setBenoetigeTf(int value)
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

Art Fahrtag::getArt() const
{
    return art;
}

void Fahrtag::setArt(const Art &value)
{
    art = value;
    emit changed(this);
}
