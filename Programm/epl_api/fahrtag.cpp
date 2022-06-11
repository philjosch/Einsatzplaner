#include "fahrtag.h"
#include "wagen.h"
#include "verteiler.h"
#include "basics.h"
#include "export.h"

#include <QJsonObject>
#include <QJsonArray>

Fahrtag::Fahrtag(QDate date, ManagerPersonal *p): AActivity(date, p)
{
    art = Museumszug;
    setZeitAnfang(QTime(8, 45));
    setZeitAnfang(QTime(8, 15), Tf);
    setZeitEnde(QTime(18, 45));
    setPersonalBenoetigt(1, Tf);
    setPersonalBenoetigt(1, Zf);
    setPersonalBenoetigt(-1, Zub);
    setPersonalBenoetigt(-1, Service);
    setPersonalBenoetigt(0);
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
    setZeitAnfang(QTime::fromString(o.value("zeitTf").toString(), "hh:mm"), Tf);
    if (! o.value("benoetigeTf").isBool()) {
        setPersonalBenoetigt(o.value("benoetigeTf").toInt(1), Tf);
    } else {
        if (o.value("benoetigeTf").toBool(true)) {
            setPersonalBenoetigt(1, Tf);
        } else {
            setPersonalBenoetigt(0, Tf);
        }
    }
    setPersonalBenoetigt(o.value("benoetigeZf").toBool(true)?1:0, Zf);
    setPersonalBenoetigt(o.value("benoetigeZub").toBool(true)?-1:0, Zub);
    setPersonalBenoetigt(o.value("benoetigeService").toBool(true)?-1:0, Service);

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
    for(Reservierung *r: qAsConst(reservierungen)) {
        delete r;
    }
    for(Wagen *w: qAsConst(wagen)) {
        delete w;
    }
}

QJsonObject Fahrtag::toJson() const
{
    QJsonObject o = AActivity::toJson();
    o.insert("art", art);
    o.insert("zeitTf", getZeitAnfang(Tf).toString("hh:mm"));
    o.insert("benoetigeTf", getPersonalBenoetigt(Tf));
    o.insert("benoetigeZf", getPersonalBenoetigt(Zf) != 0);
    o.insert("benoetigeZub", getPersonalBenoetigt(Zub) != 0);
    o.insert("benoetigeService", getPersonalBenoetigt(Service) != 0);
    o.insert("wagenreihung", wagenreihung);
    QJsonArray array;
    for(Reservierung *r: reservierungen.values()) {
        array.append(r->toJson());
    }
    o.insert("reservierungen", array);
    o.insert("checkAll", checkAll);
    return o;
}

QString Fahrtag::getHtmlForSingleView() const
{
    QString required = "<font color='"+COLOR_REQUIRED+"'>%1</font>";
    QString html = "";
    // Überschrift
    html += "<h2 class='pb'>";
    html += toString(art);
    html += " am " + QLocale().toString(datum, "dddd, dd.MM.yyyy");
    if (abgesagt)
        html += required.arg(" ABGESAGT!");
    if (wichtig)
        html += required.arg(" WICHTIG!");
    html += "</h2>";
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
        html += "<p><b>Dienstzeiten</b>:<br/>Beginn Tf: "+getZeitAnfang(Tf).toString("hh:mm")+"<br/>Beginn allg.: "+getZeitAnfang().toString("hh:mm")+"<br/>";
        if (getBis() <= QDateTime::currentDateTime()) {
            html += "Ende: "+getZeitEnde().toString("hh:mm")+"</p>";
        } else {
            html += "Geplantes Ende: "+getZeitEnde().toString("hh:mm")+"</p>";
        }
    }

    // Personal
    QList<Einsatz*> tf;
    QList<Einsatz*> zf;
    QList<Einsatz*> zub;
    QList<Einsatz*> begl;
    QList<Einsatz*> service;
    QList<Einsatz*> sonstige;

    // Aufsplitten der Personen auf die Einzelnen Listen
    for(Einsatz *e: personen) {
        switch (e->getKategorie()) {
        case Tf:
        case Tb: tf.append(e); break;
        case Zf: zf.append(e); break;
        case Zub: zub.append(e); break;
        case Begleiter: begl.append(e); break;
        case Service: service.append(e); break;
        default: sonstige.append(e); break;
        }
    }
    // *Tf/Tb
    if (getPersonalBenoetigt(Tf) || tf.size() > 0) {
        html += "<p><b>";
        if (getPersonalBenoetigt(Tf)) {
            html += required.arg("Noch "+QString::number(getPersonalBenoetigt(Tf)))
                    +" Triebfahrzeugführer (Tf) "
                    +required.arg("benötigt");
        } else {
            html += "Triebfahrzeugführer (Tf)";
        }
        html += ":</b><br/>"+listToString(" | ", tf)+"</p>";
    }
    // *Zf
    if ((getPersonalBenoetigt(Zf) && (art != Schnupperkurs)) || zf.size() > 0) {
        html += "<p><b>Zugführer";
        html += (getPersonalBenoetigt(Zf) && (art != Schnupperkurs) ? required.arg(" wird benötigt"):"");
        html += ":</b><br/>"+listToString(" | ", zf)+"</p>";
    }
    // *Zub, Begl.o.b.A
    if ((getPersonalBenoetigt(Zub) && (art != Schnupperkurs)) || (zub.size() > 0 || begl.size() > 0)) {
        html += "<p><b>Zugbegleiter und <i><b>Begleiter ohne betriebliche Ausbildung</b></i>";
        html += (getPersonalBenoetigt(Zub) && (art != Schnupperkurs) ? required.arg(" werden benötigt"):"");
        html += ":</b><br/>";
        html += listToString(" | ", zub);
        // Begl. o.b.A
        if (! zub.isEmpty() && ! begl.isEmpty())
            html += " | ";
        html += "<i>" + listToString(" | ", begl) + "</i></p>";
    }
    // *Service
    if ((getPersonalBenoetigt(Service) && (art != Schnupperkurs)) || service.size() > 0) {
        html += "<p><b>Service-Personal";
        html += (getPersonalBenoetigt(Service) && (art != Schnupperkurs) ? required.arg(" wird benötigt"):"");
        html += ":</b><br/>"+listToString(" | ", service) +"</p>";
    }
    // *Sonstiges personal
    if (sonstige.size() > 0) {
        html += "<p><b>Sonstiges Personal";
        html += (getPersonalBenoetigt() ? required.arg(" wird benötigt"):"");
        html += ":</b><br/>"+listToString(" | ", sonstige, "", "", true) +"</p>";
    }
    if (bemerkungen != "") {
        html += "<p><b>Bemerkungen:</b><br/>"+QString(bemerkungen).replace("\n", "<br/>")+"</p>";
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

QString Fahrtag::getHtmlForTableView() const
{
    bool zeilenUmbruch = false;

    QString html = "<tr bgcolor='"+getFarbe()+"'>";
    // Datum, Anlass
    if (wichtig) {
        html += "<td bgcolor='#ff8888'>";
    } else {
        html += "<td>";
    }
    html += "<b>"+QLocale().toString(datum, "dddd d.M.yyyy")+"</b><br/>";
    html += toString(art);
    if (anlass != "") {
        html += ": <i>"+QString(anlass).replace("\n", "<br/>")+"</i>";
    }

    if (abgesagt) {
        html += "</td><td colspan='4'><b>Abgesagt!</b></td>";
        html += "<td>"+QString(bemerkungen).replace("\n", "<br/>")+"</td></tr>";
        return html;
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
        html += "<td>Beginn Tf: "+getZeitAnfang(Tf).toString("hh:mm") + "<br/>";
        if (art != Schnupperkurs) {
            html += "Beginn allg.: "+getZeitAnfang().toString("hh:mm") + "<br/>";
        }
        if (datum < QDate::currentDate()) {
            html += "Ende: "+getZeitEnde().toString("hh:mm") + "</td>";
        } else {
            html += "Ende: ~"+getZeitEnde().toString("hh:mm") + "</td>";
        }
    }

    QMap<Category, QList<Einsatz*>*> gruppen = splitNachKategorie();

    // Tf, Tb
    QString beginnZelleBenoetigt = "<td>";
    if (QDate::currentDate().addDays(10) >= datum && getVon() >= QDateTime::currentDateTime()) {
        beginnZelleBenoetigt = "<td bgcolor='#ff8888'>";
    }
    QString benoetigt = "<b>%1</b>";
    if (getPersonalBenoetigt(Tf)) {
        html += beginnZelleBenoetigt;
        if (gruppen.value(Tf)->size() > 0)
            html += benoetigt.arg("Noch %2 Lokführer benötigt!").arg(getPersonalBenoetigt(Tf));
        else
            html += benoetigt.arg("%2 Lokführer benötigt!").arg(getPersonalBenoetigt(Tf));

    } else {
        html += "<td>";
    }
    if (gruppen.value(Tf)->size() + gruppen.value(Tb)->size() > 0) {
        html += "<ul>" + listToString("", *gruppen.value(Tf), "<li>", "</li>");
        html += listToString("", *gruppen.value(Tb), "<li><i>", "</i></li>") + "</ul>";
        gruppen.remove(Tf);
        gruppen.remove(Tb);
    }
    html += "</td>";

    // Zf, Zub, Begl.o.b.A.
    if ((getPersonalBenoetigt(Zf) || getPersonalBenoetigt(Zub)) && (art != Schnupperkurs)) {
        html += beginnZelleBenoetigt;
    } else {
        html += "<td>";
    }
    if (getPersonalBenoetigt(Zf) && (art != Schnupperkurs)) {
        html += "<u>"+benoetigt.arg("Zugführer benötigt!")+"</u>";
        zeilenUmbruch = true;
    }
    if (getPersonalBenoetigt(Zub) && (art != Schnupperkurs)) {
        if(zeilenUmbruch) html += "<br/>";
        html += "<i>"+benoetigt.arg("Begleitpersonal benötigt!")+"</i>";
    }
    if (gruppen.value(Zf)->size() + gruppen.value(Zub)->size() + gruppen.value(Begleiter)->size() > 0) {
        html += "<ul>";
        html += listToString("", *gruppen.value(Zf), "<li><u>", "</u></li>");
        html += listToString("", *gruppen.value(Zub), "<li>", "</li>");
        html += listToString("", *gruppen.value(Begleiter), "<li><i>", "</i></li>");
        html += "</ul>";
        gruppen.remove(Zf);
        gruppen.remove(Zub);
        gruppen.remove(Begleiter);
    }
    html += "</td>";

    // Service
    if (getPersonalBenoetigt(Service) && (art != Schnupperkurs)) {
        html += beginnZelleBenoetigt;
        html += benoetigt.arg("Service-Personal benötigt!");
    } else {
        html += "<td>";
    }
    if (gruppen.value(Service)->size() > 0) {
        html += "<ul>" + listToString("", *gruppen.value(Service), "<li>", "</li>") + "</ul>";
        gruppen.remove(Service);
    }
    html += "</td>";

    // Sonstiges
    zeilenUmbruch = false;
    if (getPersonalBenoetigt()) {
        html += beginnZelleBenoetigt;
        html += benoetigt.arg("Sonstiges Personal wird benötigt!");
        zeilenUmbruch = true;
    } else {
        html += "<td>";
    }

    for(auto it = gruppen.cbegin(); it != gruppen.cend(); ++it) {
        if (it.key() != Sonstiges)
            gruppen.value(Sonstiges)->append(*it.value());
    }
    if (gruppen.value(Sonstiges)->size() > 0) {
        html += "<ul>";
        html += listToString("", *gruppen.value(Sonstiges), "<li>", "</li>", true);
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
        html += QString(bemerkungen).replace("\n", "<br/>");
    }

    html += "</td>";
    html += "</tr>";
    return html;
}

bool Fahrtag::printReservierungsuebersicht(QPrinter *printer) const
{
    QString a = "<h3>";
    a += toString(art)+" am "+QLocale().toString(datum, "dddd dd. MM. yyyy");
    a += " - Die Reservierungen</h3>";
    a += "<table cellspacing='0' width='100%'><thead><tr> <th>Name</th> <th>Anzahl</th> <th>Sitzplätze</th> <th>Sonstiges</th></tr></thead><tbody>";
    // Sortieren der Daten nach Wagenreihung
    QStringList wagen = wagenreihung.split(QRegularExpression("\\s*,\\s*"));
    QList<int> wagenNummern;
    for(const QString &s: qAsConst(wagen))
        wagenNummern.append(s.toInt());

    // Sortieren der Reservierungen
    QHash<int, QList<Reservierung*>> wagenZuRes;
    for (Reservierung *r: reservierungen) {
        for(int i: r->getSitzplatz().keys()) {
            if (!wagenZuRes.contains(i))
                wagenZuRes.insert(i, QList<Reservierung*>());
            int pos = 0;
            QList<Reservierung*> list= wagenZuRes.value(i);
            list.insert(0, r);
            while (pos+1 < list.length() && list.at(pos)->getName() > list.at(pos+1)->getName()) {
                list.swapItemsAt(pos, pos+1);
                pos++;
            }
            wagenZuRes.insert(i, list);
        }
        if (r->getSitzplatz().isEmpty()) {
            if (! wagenZuRes.contains(999))
                wagenZuRes.insert(999, QList<Reservierung*>());
            int pos = 0;
            QList<Reservierung*> list= wagenZuRes.value(999);
            list.insert(0, r);
            while (pos+1 < list.length() && list.at(pos)->getName() > list.at(pos+1)->getName()) {
                list.swapItemsAt(pos, pos+1);
                pos++;
            }
            wagenZuRes.insert(999, list);
        }
    }

    for(int wagenNr: wagenNummern) {
        if (wagenZuRes.contains(wagenNr)) {
            a += "<tr><td columnspan='4'><b>Wagen "+QString::number(wagenNr)+":</b></td></tr>";
            for(Reservierung *r: wagenZuRes.value(wagenNr)) {
                a += r->getHtmlForDetailTable();
            }
        }
    }
    if (wagenZuRes.contains(999)) {
        a += "<tr><td columnspan='4'><b>Reservierungen ohne Sitzplätze:</b></td></tr>";
        for(Reservierung *r: wagenZuRes.value(999)) {
            a += r->getHtmlForDetailTable();
        }
    }

    a += "</tbody></table>";

    return Export::druckeHtml(a + Export::zeitStempel(), printer);
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

int Fahrtag::getBelegung(int klasse, int zug) const
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

int Fahrtag::getKapazitaet(int klasse) const
{
    int summe = 0;
    for(Wagen *w: wagen) {
        if ((klasse == -1) || (w->klasse() == klasse) || (klasse == 0 && (w->klasse() == 2 || w->klasse() == 3)))
            summe += w->getKapazitaet();
    }
    return summe;
}

int Fahrtag::getAnzahlReservierungen() const
{
    return reservierungen.size();
}

QSet<Reservierung *> Fahrtag::getReservierungen() const
{
    return reservierungen;
}

QList<Mistake> Fahrtag::verteileSitzplaetze()
{
    // aufteilen der Wagen auf die beiden Gruppen
    QList<Wagen*> ersteKlasse = QList<Wagen*>();
    QList<Wagen*> andereKlasse = QList<Wagen*>();
    QStringList wagenR = wagenreihung.split(QRegularExpression("\\s*,\\s*"));
    for(const QString &s: qAsConst(wagenR)) {
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
    for(Reservierung *r: qAsConst(reservierungen)) {
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
    for(Reservierung *r: qAsConst(reservierungen)) {
        r->setSitzplatz(r->getSitzplatz());
    }

    return {okAndere, okErste};
}

bool Fahrtag::checkPlaetze(QMap<int, QList<int>> p, Reservierung *r) const
{
    // Überprüft, ob die eingegebenen Sitzplätze frei sind, oder ob sie schon belegt wurden
    int summe = 0;
    for (auto it = p.cbegin(); it != p.cend(); ++it) {
        if (! nummerToWagen.contains(it.key())) return false;
        if (r->getKlasse() == 1 && nummerToWagen.value(it.key())->klasse() != 1) return false;
        if (r->getKlasse() == 0 && (nummerToWagen.value(it.key())->klasse() != 2 &&
                                    nummerToWagen.value(it.key())->klasse() != 3)) return false;
        summe += it.value().length();
        // für jeden Wagen prüfen, ob die Plätze belegt sind
        if (! nummerToWagen.value(it.key())->testPlaetze(it.value(), r)) return false;
    }
    if (summe != r->getAnzahl()) return false;
    return true;
}

bool Fahrtag::checkPlaetze(QString p, Reservierung *r) const
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
    for(Wagen *w: qAsConst(wagen)) {
        wagenNummer.insert(w->getNummer(), w);
    }

    QList<Wagen*> wagenNeu = QList<Wagen*>();
    QStringList wagenSplit = wagenreihung.split(QRegularExpression("\\s*,\\s*"));
    for(const QString &s: qAsConst(wagenSplit)) {
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
        for(Wagen *wagen: wagenNummer) {
            if (! wagen->isEmpty())
                return false;
        }
        // Wenn test für alle Wagen überstanden ist, können sie gelöscht werden
    }
    wagen = wagenNeu;
    nummerToWagen.clear();
    for(Wagen *w: qAsConst(wagen)) {
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

bool Fahrtag::checkPlausibilitaet(QList<int> zuege, QList<int> haltepunkte) const
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
