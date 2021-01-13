#include "fahrtag.h"
#include "wagen.h"
#include "verteiler.h"
#include "basics.h"

#include <QJsonObject>
#include <QJsonArray>

Fahrtag::Fahrtag(QDate date, ManagerPersonal *p): AActivity(date, p)
{
    art = Museumszug;
    wichtig = false;
    zeitTf = QTime(8, 15);
    zeitAnfang = QTime(8, 45);
    zeitEnde = QTime(18, 45);
    benoetigeTf = 1;
    benoetigeZf = true;
    benoetigeZub = true;
    benoetigeService = true;
    personalBenoetigt = false;
    wagenreihung = "221, 217, 208, 309";
    reservierungen = QSet<Reservierung*>();
    checkAll = false;

    wagen = QList<Wagen *>();
    nummerToWagen = QMap<int, Wagen*>();
    createWagen();
}

Fahrtag::Fahrtag(QJsonObject o, ManagerPersonal *p) : AActivity(o, p)
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

    wagenreihung = o.value("wagenreihung").toString();
    reservierungen = QSet<Reservierung*>();
    checkAll = o.value("checkAll").toBool(false);

    wagen = QList<Wagen*>();
    nummerToWagen = QMap<int, Wagen*>();
    createWagen();

    QJsonArray array = o.value("reservierungen").toArray();
    for(int i = 0; i < array.size(); i++) {
        Reservierung *r = new Reservierung(array.at(i).toObject(), &nummerToWagen);
        reservierungen.insert(r);
        connect(r, &Reservierung::changed, this, [=]() { emit changed(this); });
    }
}

Fahrtag::~Fahrtag()
{
    for(Reservierung *r: reservierungen) {
        delete r;
    }
    for(Wagen *w: wagen) {
        delete w;
    }
}

QJsonObject Fahrtag::toJson()
{
    QJsonObject o = AActivity::toJson();
    o.insert("art", art);
    o.insert("zeitTf", zeitTf.toString("hh:mm"));
    o.insert("wichtig", wichtig);
    o.insert("benoetigeTf", benoetigeTf);
    o.insert("benoetigeZf", benoetigeZf);
    o.insert("benoetigeZub", benoetigeZub);
    o.insert("benoetigeService", benoetigeService);
    o.insert("wagenreihung", wagenreihung);
    QJsonArray array;
    for(Reservierung *r: reservierungen.values()) {
        array.append(r->toJson());
    }
    o.insert("reservierungen", array);
    o.insert("checkAll", checkAll);
    return o;
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

Infos Fahrtag::getIndividual(Person *person, Category kat)
{
    Infos alt = AActivity::getIndividual(person, kat);
    Infos neu = Infos();
    neu.bemerkung = alt.bemerkung;
    neu.kategorie = alt.kategorie;
    neu.beginn = alt.beginn;
    neu.ende = alt.ende;

    if (!zeitenUnbekannt) {
        if ((alt.beginn == zeitAnfang) && (alt.kategorie == Tf)) {
            neu.beginn = zeitTf;
        }
    }
    return neu;
}

QString Fahrtag::getHtmlForSingleView()
{
    QString required = "<font color='"+COLOR_REQUIRED+"'>%1</font>";
    QString html = "";
    // Überschrift
    html += "<h2 class='pb'>" +getStringFromArt(art) + " am " + datum.toString("dddd, dd.MM.yyyy")+(wichtig?required.arg(" WICHTIG!"):"")+"</h2>";
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
    for(Einsatz e: personen.keys()) {
        Person *p = e.person;
        switch (e.cat) {
        case Tf:
        case Tb: tf.insert(p, personen.value(e)); break;
        case Zf: zf.insert(p, personen.value(e)); break;
        case Zub: zub.insert(p, personen.value(e)); break;
        case Begleiter: begl.insert(p, personen.value(e)); break;
        case Service: service.insert(p, personen.value(e)); break;
        default: sonstige.insert(p, personen.value(e)); break;
        }
    }
    // *Tf/Tb
    if (benoetigeTf || tf.size() > 0) {
        html += "<p><b>";
        if (benoetigeTf) {
            html += required.arg("Noch "+QString::number(benoetigeTf))
                    +" Triebfahrzeugführer (Tf) "
                    +required.arg("benötigt");
        } else {
            html += "Triebfahrzeugführer (Tf)";
        }
        html += ":</b><br/>"+listToString(" | ", tf)+"</p>";
    }
    // *Zf
    if ((benoetigeZf && (art != Schnupperkurs)) || zf.size() > 0) {
        html += "<p><b>Zugführer";
        html += (benoetigeZf && (art != Schnupperkurs) ? required.arg(" wird benötigt"):"");
        html += ":</b><br/>"+listToString(" | ", zf)+"</p>";
    }
    // *Zub, Begl.o.b.A
    if ((benoetigeZub && (art != Schnupperkurs)) || (zub.size() > 0 || begl.size() > 0)) {
        html += "<p><b>Zugbegleiter und <i><b>Begleiter ohne betriebliche Ausbildung</b></i>";
        html += (benoetigeZub && (art != Schnupperkurs) ? required.arg(" werden benötigt"):"");
        html += ":</b><br/>";
        html += listToString(" | ", zub);
        // Begl. o.b.A
        if (! zub.isEmpty() && ! begl.isEmpty())
            html += " | ";
        html += "<i>" + listToString(" | ", begl) + "</i></p>";
    }
    // *Service
    if ((benoetigeService && (art != Schnupperkurs)) || service.size() > 0) {
        html += "<p><b>Service-Personal";
        html += (benoetigeService && (art != Schnupperkurs) ? required.arg(" wird benötigt"):"");
        html += ":</b><br/>"+listToString(" | ", service) +"</p>";
    }
    // *Sonstiges personal
    if (sonstige.size() > 0) {
        html += "<p><b>Sonstiges Personal";
        html += (personalBenoetigt ? required.arg(" wird benötigt"):"");
        html += ":</b><br/>"+listToString(" | ", sonstige, "", "", true) +"</p>";
    }
    if (bemerkungen != "") {
        html += "<p><b>Bemerkungen:</b><br/>"+bemerkungen.replace("\n", "<br/>")+"</p>";
    }

    // Reservierungen
    if (getAnzahlReservierungen() > 0) {
        html += "<p><b>Reservierungen:</b>";
        QString helperRes = "<br/>Zug %1: %2 Plätze in 1.Klasse, %3 Plätze in 2./3.Klasse";
        if (getBelegung(-1, 2201) > 0)
            html += helperRes.arg(2201).arg(getBelegung(1, 2201)).arg(getBelegung(0, 2201));
        if (getBelegung(-1, 2202) > 0)
            html += helperRes.arg(2202).arg(getBelegung(1, 2202)).arg(getBelegung(0, 2202));
        if (getBelegung(-1, 2203) > 0)
            html += helperRes.arg(2203).arg(getBelegung(1, 2203)).arg(getBelegung(0, 2203));
        if (getBelegung(-1, 2204) > 0)
            html += helperRes.arg(2204).arg(getBelegung(1, 2204)).arg(getBelegung(0, 2204));
        if (getBelegung(-1, 2205) > 0)
            html += helperRes.arg(2205).arg(getBelegung(1, 2205)).arg(getBelegung(0, 2205));
        if (getBelegung(-1, 2206) > 0)
            html += helperRes.arg(2206).arg(getBelegung(1, 2206)).arg(getBelegung(0, 2206));
        if (getBelegung(-1, 0) > 0)
            html += QString("<br/>Gesamt: %1 Plätze 1.Klasse, %2 Plätze 2./3.Klasse <br/>").arg(getBelegung(1)).arg(getBelegung(0));
        html += "</p>";

        if (art != Nikolauszug) {
            html += "<table cellspacing='0' width='100%'><thead><tr><th>Kontakt</th><th>Sitzplätze</th><th>Ein- und Ausstieg</th><th>Sonstiges</th></tr></thead><tbody>";
            for(Reservierung *r: reservierungen) {
                html += r->getHtmlForTable();
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
    html += "<b>"+datum.toString("dddd d.M.yyyy")+"</b><br/>";
    html += getStringFromArt(art);
    if (anlass != "") {
        html += ":<br/><i>"+anlass.replace("\n", "<br/>")+"</i>";
    }
    // Wagenreihung
    if (wagenreihung != "") {
        html += "<br/>"+ wagenreihung;
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

    QMap<Person*, Infos> tf;
    QMap<Person*, Infos> zf;
    QMap<Person*, Infos> zub;
    QMap<Person*, Infos> begl;
    QMap<Person*, Infos> service;
    QMap<Person*, Infos> sonstige;

    // Aufsplitten der Personen auf die Einzelnen Listen
    for(Einsatz e: personen.keys()) {
        switch (e.cat) {
        case Tf:
        case Tb: tf.insert(e.person, personen.value(e)); break;
        case Zf: zf.insert(e.person, personen.value(e)); break;
        case Zub: zub.insert(e.person, personen.value(e)); break;
        case Begleiter: begl.insert(e.person, personen.value(e)); break;
        case Service: service.insert(e.person, personen.value(e)); break;
        default: sonstige.insert(e.person, personen.value(e)); break;
        }
    }

    // Tf, Tb
    QString beginnZelleBenoetigt = "<td>";
    if (QDate::currentDate().addDays(10) >= datum && datum >= QDate::currentDate()) {
        beginnZelleBenoetigt = "<td bgcolor='#ff8888'>";
    }
    QString benoetigt = "<b>%1</b>";
    if (benoetigeTf > 0) {
        html += beginnZelleBenoetigt;
        html += benoetigt.arg("%2 Lokführer benötigt!").arg(benoetigeTf);
    } else {
        html += "<td>";
    }
    if (tf.size() > 0) {
        html += "<ul>" + listToString("", tf, "<li>", "</li>") + "</ul>";
    }
    html += "</td>";

    // Zf, Zub, Begl.o.b.A.
    if (benoetigeZf && (art != Schnupperkurs)) {
        html += beginnZelleBenoetigt;
        html += "<u>"+benoetigt.arg("Zugführer benötigt!")+"</u><br/>";
    } else {
        html += "<td>";
    }
    if (benoetigeZub && (art != Schnupperkurs)) {
        html += "<i>"+benoetigt.arg("Begleitpersonal benötigt!")+"</i>";
    }
    html += "<ul>";
    if (zf.size() > 0) {
        html += listToString("", zf, "<li><u>", "</u></li>");
    }
    if (zub.size() > 0) {
        html += listToString("", zub, "<li>", "</li>");
    }
    if (begl.size() > 0) {
        html += listToString("", begl, "<li><i>", "</i></li>");
    }
    html += "</ul></td>";

    // Service
    if (benoetigeService && (art != Schnupperkurs)) {
        html += beginnZelleBenoetigt;
        html += benoetigt.arg("Service-Personal benötigt!");
    } else {
        html += "<td>";
    }
    if (service.size() > 0) {
        html += "<ul>" + listToString("", service, "<li>", "</li>") + "</ul>";
    }
    html += "</td>";

    // Sonstiges
    bool zeilenUmbruch = false;
    if (personalBenoetigt) {
        html += beginnZelleBenoetigt;
        html += benoetigt.arg("Sonstiges Personal wird benötigt!");
        zeilenUmbruch = true;
    } else {
        html += "<td>";
    }
    if (sonstige.size() > 0) {
        html += "<ul>";
        html += listToString("", sonstige, "<li>", "</li>", true);
        html += "</ul>";
        zeilenUmbruch = false;
    }
    // Sneek-Peek Reservierungen
    if (art != Schnupperkurs && art != Gesellschaftssonderzug && getBelegung(-1) > 0) {
        if(zeilenUmbruch) html += "<br/>";
        zeilenUmbruch = true;
        html += tr("%1 %2 bei %3 %4")
                .arg(getBelegung(-1))
                .arg(getBelegung(-1) == 1 ? tr("reservierter Sitzplatz"): tr("reservierte Sitzplätze"))
                .arg(getAnzahlReservierungen())
                .arg(getAnzahlReservierungen() == 1 ? tr("Reservierung") : tr("Reservierungen"));
    }

    // Bemerkungen
    if (bemerkungen != "") {
        if (zeilenUmbruch) html += "<br/>";
        html += bemerkungen.replace("\n", "<br/>");
    }

    html += "</td>";
    html += "</tr>";
    return html;
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
    for (Reservierung *r: reservierungen) {
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

QString Fahrtag::getWagenreihung() const
{
    return wagenreihung;
}
bool Fahrtag::setWagenreihung(const QString &value)
{
    QString old = wagenreihung;
    wagenreihung = value;
    if (createWagen()) {
        emit changed(this);
        return true;
    }
    wagenreihung = old;
    return false;
}

int Fahrtag::getBelegung(int klasse, int zug)
{
    int summe = 0;
    for(Reservierung *r: reservierungen) {
        if (klasse == -1 || (r->getKlasse() == klasse)) {
            if (zug == 0 || r->inZug(zug)) {
                summe += r->getAnzahl();
            }
        }
    }
    return summe;
}

int Fahrtag::getKapazitaet(int klasse)
{
    int summe = 0;
    for(Wagen *w: wagen) {
        if ((klasse == -1) || (w->klasse() == klasse) || (klasse == 0 && (w->klasse() == 2 || w->klasse() == 3)))
            summe += w->getKapazitaet();
    }
    return summe;
}

int Fahrtag::getAnzahlReservierungen()
{
    return reservierungen.size();
}

QSet<Reservierung *> Fahrtag::getReservierungen()
{
    return reservierungen;
}

QList<Mistake> Fahrtag::verteileSitzplaetze()
{
    // aufteilen der Wagen auf die beiden Gruppen
    QList<Wagen*> ersteKlasse = QList<Wagen*>();
    QList<Wagen*> andereKlasse = QList<Wagen*>();
    QStringList wagenR = wagenreihung.split(QRegExp("\\s*,\\s*"));
    for(QString s: wagenR) {
        int nummer = s.toInt();
        switch (Wagen::klasse(nummer)) {
        case 1: ersteKlasse.append(new Wagen(nummer)); break;
        case 2:
        case 3: andereKlasse.append(new Wagen(nummer)); break;
        default: break;
        }
    }
    // Aufteilen der Resevierungen auf die beiden gruppen
    QSet<Reservierung*> resErste = QSet<Reservierung*>();
    QSet<Reservierung*> resAndere = QSet<Reservierung*>();
    for(Reservierung *r: reservierungen) {
        if (r->getKlasse() == 1)
            resErste.insert(r);
        else
            resAndere.insert(r);
    }

    // Verteilen der Sitzplätze

    // Erste klasse verteilen
    Mistake okErste = Mistake::OK;
    if (resErste.size() > 0 && ersteKlasse.length() > 0) {
        Verteiler erste = Verteiler(ersteKlasse, resErste, checkAll);
        okErste = erste.verteile();
    } else if (resErste.size() > 0 && ersteKlasse.length() == 0) {
        okErste = Mistake::KapazitaetUeberlauf;
    }

    // 2./3. Klasse verteilen
    Mistake okAndere = Mistake::OK;
    if (resAndere.size() > 0 && andereKlasse.length() > 0) {
        Verteiler andere = Verteiler(andereKlasse, resAndere, checkAll);
        okAndere = andere.verteile();
    } else if (resAndere.size() > 0 && andereKlasse.length() == 0) {
        okAndere = Mistake::KapazitaetUeberlauf;
    }

    // Die Sitzplätze zuweisen, sodass sie in den Wagen erscheinen.
    for(Reservierung *r: reservierungen.values()) {
        r->setSitzplatz(r->getSitzplatz());
    }

    return {okAndere, okErste};
}

bool Fahrtag::checkPlaetze(QMap<int, QList<int>> p, Reservierung *r)
{
    // Überprüft, ob die eingegebenen Sitzplätze frei sind, oder ob sie schon belegt wurden
    int summe = 0;
    for(int w: p.keys()) {
        if (! nummerToWagen.contains(w)) return false;
        if (r->getKlasse() == 1 && nummerToWagen.value(w)->klasse() != 1) return false;
        if (r->getKlasse() == 0 && (nummerToWagen.value(w)->klasse() != 2 &&
                                    nummerToWagen.value(w)->klasse() != 3)) return false;
        summe += p.value(w).length();
        // für jeden Wagen prüfen, ob die Plätze belegt sind
        if (! nummerToWagen.value(w)->testPlaetze(p.value(w), r)) return false;
    }
    if (summe != r->getAnzahl()) return false;
    return true;
}

bool Fahrtag::checkPlaetze(QString p, Reservierung *r)
{
    return checkPlaetze(Reservierung::getPlaetzeFromString(p), r);
}

Reservierung *Fahrtag::createReservierung()
{
    Reservierung *r = new Reservierung(&nummerToWagen);
    reservierungen.insert(r);
    connect(r, &Reservierung::changed, this, [=]() { emit changed(this); });
    emit changed(this);
    return r;
}

bool Fahrtag::removeReservierung(Reservierung *res)
{
    bool ok = reservierungen.remove(res);
    delete res;
    emit changed(this);
    return ok;
}

bool Fahrtag::createWagen()
{
    QHash<int,Wagen*> wagenNummer;
    for(Wagen *w: wagen) {
        wagenNummer.insert(w->getNummer(), w);
    }

    QList<Wagen*> wagenNeu = QList<Wagen*>();
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
        wagenNeu.append(w);
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
    nummerToWagen.clear();
    for(Wagen *w: wagen) {
        nummerToWagen.insert(w->getNummer(), w);
    }
    return true;
}

bool Fahrtag::getCheckAll() const
{
    return checkAll;
}
void Fahrtag::setCheckAll(bool value)
{
    checkAll = value;
    emit changed(this);
}

bool Fahrtag::checkPlausibilitaet(QList<int> zuege, QList<int> haltepunkte)
{
    if (zuege.length() != haltepunkte.length()) return false;
    if (zuege.length() > 4) return false;

    int z1 = zuege.at(0);
    int z2 = zuege.at(1);
    int z3 = zuege.at(2);
    int z4 = zuege.at(3);

    int h1 = haltepunkte.at(0);
    int h2 = haltepunkte.at(1);
    int h3 = haltepunkte.at(2);
    int h4 = haltepunkte.at(3);

    bool z_ok = ( z1 <= z2 || z1 == 0 || z2 == 0)
            && (z2 < z3 || z2 == 0 || z3 == 0)
            && (z3 <= z4 || z3 == 0 || z4 == 0)
            && (z1 < z3 || z1 == 0 || z3 == 0)
            && (z2 < z4 || z2 == 0 || z4 == 0)
            && (z1 < z4 || z1 == 0 || z4 == 0);
    bool h_ok1 = (h1 != h2 || h1 == -1 || h2 == -1 || z1 < z2)
            && (z1 != z2 || z1 == 0 || z2 == 0 || h1 == -1 || h2 == -1 || ((z1 %2 != 0 || h1 < h2) && (z1%2 != 1 || h1 > h2)))
            && (h1 == -1 || z1 == 0 || ((z1%2 != 0 || h1 != 10) && (z1%2 != 1 || h1 !=  0)))
            && (h2 == -1 || z2 == 0 || ((z2%2 != 0 || h2 !=  0) && (z2%2 != 1 || h2 != 10)));
    bool h_ok2 = (h3 != h4 || h3 == -1 || h4 == -1 || z3 < z4)
            && (z3 != z4 || z3 == 0 || z4 == 0 || h3 == -1 || h4 == -1 || ((z3 %2 != 0 || h3 < h4) && (z3%2 != 1 || h3 > h4)))
            && (h3 == -1 || z3 == 0 || ((z3%2 != 0 || h3 != 10) && (z3%2 != 1 || h3 !=  0)))
            && (h4 == -1 || z4 == 0 || ((z4%2 != 0 || h4 !=  0) && (z4%2 != 1 || h4 != 10)));
    return (z_ok && h_ok1 && h_ok2);
}
