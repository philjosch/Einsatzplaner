#include "aactivity.h"
#include "person.h"
#include "eplexception.h"
#include "export.h"

#include <QJsonArray>
#include <QLinkedList>

using namespace EplException;

const QStringList AActivity::EXTERNAL_LIST = QStringList() << "Extern" << "Führerstand" << "FS" << "Schnupperkurs" << "ELF" << "Ehrenlokführer" << "ELF-Kurs";
const QStringList AActivity::QUALIFICATION_LIST = QStringList() << "Azubi" << "Ausbildung" << "Tf-Ausbildung" << "Zf-Ausbildung" << "Tf-Unterricht" << "Zf-Unterricht" << "Weiterbildung";
const QString AActivity::COLOR_REQUIRED = "#ff3333";

const QString AActivity::KOPF_LISTE_HTML = "<h3>Übersicht über die Aktivitäten</h3>"
                                         "<table cellspacing='0' width='100%'><thead><tr>"
                                         "<th>Datum, Anlass</th> <th>Dienstzeiten</th>"
                                         "<th>Tf, <i>Tb</i></th> <th><u>Zf</u>, Zub, <i>Begl.o.b.A</i></th> <th>Service</th>"
                                         "<th>Sonstiges</th> </tr></thead><tbody>";
const QString AActivity::FUSS_LISTE_HTML = "</tbody></table>";

AActivity::AActivity(QDate date, ManagerPersonal *p) : QObject()
{
    art = Arbeitseinsatz;
    datum = date;
    ort = "";
    anfang = QMap<Category,QTime>();
    ende = QMap<Category,QTime>();
    setZeitAnfang(QTime(10, 0));
    if (date.month() <= 3 || date.month() >= 11) {
        setZeitEnde(QTime(16, 0));
    } else {
        setZeitEnde(QTime(18, 0));
    }
    zeitenUnbekannt = false;
    anlass = "";
    bemerkungen = "";
    personen = QList<Einsatz*>();
    personalBenoetigt = true;
    wichtig = false;
    abgesagt = false;
    personal = p;
}

AActivity::AActivity(QJsonObject o, ManagerPersonal *p) : QObject()
{
    personal = p;
    personen = QList<Einsatz*>();

    art = Arbeitseinsatz;
    datum = QDate::fromString(o.value("datum").toString(), "yyyy-MM-dd");
    ort = o.value("ort").toString();
    anfang = QMap<Category,QTime>();
    ende = QMap<Category,QTime>();
    setZeitAnfang(QTime::fromString(o.value("zeitAnfang").toString(), "hh:mm"));
    setZeitEnde(QTime::fromString(o.value("zeitEnde").toString(), "hh:mm"));
    zeitenUnbekannt = o.value("zeitenUnbekannt").toBool();
    anlass = o.value("anlass").toString();
    bemerkungen = o.value("bemerkungen").toString();
    QJsonArray array = o.value("personen").toArray();
    for(int i = 0; i < array.size(); i++) {
        QJsonObject aO = array.at(i).toObject();

        Person *person;
        if (aO.contains("id")) {
            person = p->getPersonFromID(aO.value("id").toString());
        } else if (personal->personExists(aO.value("name").toString())) {
            person = personal->getPerson(aO.value("name").toString());
        } else {
            person = new Person(aO.value("name").toString(), nullptr);
            person->setAusbildungTf(true);
            person->setAusbildungZf(true);
            person->setAusbildungRangierer(true);
        }

        Einsatz *e = new Einsatz(person, this,
                      static_cast<Category>(aO.value("kat").toInt(100)),
                      aO.value("bemerkung").toString(),
                      QTime::fromString(aO.value("beginn").toString(), "hh:mm"),
                      QTime::fromString(aO.value("ende").toString(), "hh:mm"));

        person->addActivity(e);
        personen.append(e);
    }
    personalBenoetigt = o.value("personalBenoetigt").toBool(true);
    wichtig = o.value("wichtig").toBool(false);
    abgesagt = o.value("abgesagt").toBool(false);
}

AActivity::~AActivity()
{
    for(Einsatz *e: personen) {
        e->getPerson()->removeActivity(e);
    }
}

Art AActivity::getArt() const
{
    return Art::Arbeitseinsatz;
}

QJsonObject AActivity::toJson() const
{
    QJsonObject data;
    data.insert("art", static_cast<int>(Art::Arbeitseinsatz));
    data.insert("datum", datum.toString("yyyy-MM-dd"));
    data.insert("ort", ort);
    data.insert("zeitAnfang", getZeitAnfang().toString("hh:mm"));
    data.insert("zeitEnde", getZeitEnde().toString("hh:mm"));
    data.insert("zeitenUnbekannt", zeitenUnbekannt);
    data.insert("anlass", anlass);
    data.insert("bemerkungen", bemerkungen);
    QJsonArray personenJSON;
    for(Einsatz *e: personen) {
        QJsonObject persJson;
        if (personal->personExists(e->getPerson()->getName())) {
            persJson.insert("id", e->getPerson()->getId());
        } else {
            persJson.insert("name", e->getPerson()->getName());
        }
        persJson.insert("beginn", e->getBeginnAbweichend().toString("hh:mm"));
        persJson.insert("ende", e->getEndeAbweichend().toString("hh:mm"));
        persJson.insert("kat", e->getKategorie());
        persJson.insert("bemerkung", e->getBemerkung());
        personenJSON.append(persJson);
    }
    data.insert("personen", personenJSON);
    data.insert("personalBenoetigt", personalBenoetigt);
    data.insert("wichtig", wichtig);
    data.insert("abgesagt", abgesagt);
    return data;
}

QDate AActivity::getDatum() const
{
    return datum;
}
void AActivity::setDatum(QDate value)
{
    QDate oldDate = datum;
    datum = value;
    emit changed(this, oldDate);
}

QString AActivity::getOrt() const
{
    return ort;
}
void AActivity::setOrt(const QString &value)
{
    ort = value;
    emit changed(this);
}

QDateTime AActivity::getVon([[maybe_unused]] const Category kat) const
{
    QDateTime zeit = QDateTime(datum, getZeitAnfang(kat));
    if (abgesagt)
        zeit.setTime(QTime(0,0));
    if (zeitenUnbekannt)
        zeit.setTime(QTime(0,0));
    return zeit;
}

QTime AActivity::getZeitAnfang(const Category kat) const
{
    return anfang.value(kat, anfang.value(Gesamt));
}
void AActivity::setZeitAnfang(QTime value, const Category kat)
{
    anfang.insert(kat, value);
    emit changed(this);
}

QDateTime AActivity::getBis([[maybe_unused]] const Category kat) const
{
    QDateTime zeit = QDateTime(datum, getZeitEnde(kat));
    if (abgesagt)
        zeit.setTime(QTime(0,0));
    if (zeitenUnbekannt)
        zeit.setTime(QTime(0,0));
    return zeit;
}

QTime AActivity::getZeitEnde(const Category kat) const
{
    return ende.value(kat, ende.value(Gesamt));
}
void AActivity::setZeitEnde(QTime value, const Category kat)
{
    ende.insert(kat, value);
    emit changed(this);
}

QString AActivity::getAnlass() const
{
    return anlass;
}
void AActivity::setAnlass(const QString &value)
{
    anlass = value;
    emit changed(this);
}

QString AActivity::getBemerkungen() const
{
    return bemerkungen;
}
void AActivity::setBemerkungen(const QString &value)
{
    bemerkungen = value;
    emit changed(this);
}

bool AActivity::getPersonalBenoetigt() const
{
    return personalBenoetigt;
}
void AActivity::setPersonalBenoetigt(bool value)
{
    personalBenoetigt = value;
    emit changed(this);
}

bool AActivity::getWichtig() const
{
    return wichtig;
}
void AActivity::setWichtig(bool value)
{
    wichtig = value;
    emit changed(this);
}

bool AActivity::getAbgesagt() const
{
    return abgesagt;
}
void AActivity::setAbgesagt(bool value)
{
    abgesagt = value;
    emit changed(this);
}

bool AActivity::removePerson(Einsatz *e)
{
    personen.removeAll(e);
    bool ret = false;
    if (e->getPerson() != nullptr) {
        ret = e->getPerson()->removeActivity(e);
    }
    emit changed(this);
    return ret;
}

Einsatz *AActivity::addPerson(QString p, QString bemerkung, Category kat)
{
    Person *pers = personal->getPerson(p);
    if (pers == nullptr) {
        if (!isExtern(bemerkung)) {
            throw PersonNichtGefundenException(p);
        }
        if (kat == Category::Zub) kat = Category::Begleiter;
        pers = new Person(p, nullptr);
        pers->setAusbildungTf(true);
        pers->setAusbildungZf(true);
        pers->setAusbildungRangierer(true);
    }

    /*
     * Angaben fuer die Aufgabe pruefen
     * pruefen ob die Person geeignet ist
     * Person hinzufuegen oder Fehlermeldung zurueckgeben
     */
    if (pers == nullptr)
        throw AActivityException();

    if (kat == Tf && bemerkung.contains(toString(Tb), Qt::CaseInsensitive))
        kat = Tb;
    if (kat == Zub && !hasQualification(pers, kat, bemerkung, datum)) {
        kat = Begleiter;
    }

    if (! hasQualification(pers, kat, bemerkung, datum))
        throw FalscheQualifikationException(pers->getName());

    // jetzt ist alles richtig und die person kann registiert werden.

    Einsatz *e = new Einsatz(pers, this, kat, bemerkung);

    pers->addActivity(e);
    personen.append(e);

    emit changed(this);
    return e;
}

bool AActivity::lesser(const AActivity *lhs, const AActivity *rhs)
{
    // Datum&Zeit
    if (lhs->getVon() < rhs->getVon())
        return true;
    if (lhs->getVon() > rhs->getVon())
        return false;
    if (lhs->getBis() < rhs->getBis())
        return true;
    if (lhs->getBis() > rhs->getBis())
        return false;

    // Folgender Teil zerstoert die Irreflexivitaet der Operation
    // Dieser Fall muss also extra abgefangen werden.
    if (lhs == rhs) return false;

    // Art und beliebig, bei gleicher Art
    if (lhs->getArt() != Art::Arbeitseinsatz)
        return true;
    if (rhs->getArt() == Art::Arbeitseinsatz)
        return true;
    return false;
}

QString AActivity::listToString(QString sep, QList<Einsatz*> liste, QString prefix, QString suffix, bool aufgabe) const
{
    QStringList l;
    QStringList l2;
    for(Einsatz *e: liste) {
        l2.append(e->getPerson()->getName());

        if (e->getBeginnAbweichend() == e->getVon().time() ) {
            if (e->getEndeAbweichend() == e->getBis().time() ) {
                l2.append(tr("%1 bis %2").arg(
                          e->getBeginnAbweichend().toString("HH:mm"),
                          e->getEndeAbweichend().toString("HH:mm")));
            } else {
                l2.append(tr("ab %1").arg(e->getBeginnAbweichend().toString("HH:mm")));
            }
        } else {
            if (e->getEndeAbweichend() == e->getBis().time() ) {
                l2.append(tr("bis %2").arg(e->getEndeAbweichend().toString("HH:mm")));
            }
        }

        if (e->getBemerkung() != "") {
            l2.append(e->getBemerkung());
        }
        if (aufgabe && (e->getKategorie() != Category::Sonstiges)) {
            l2.append(toString(e->getKategorie()));
        }
        l.append(prefix+l2.join("; ")+suffix);
        l2.clear();
    }
    return l.join(sep);
}

QMap<Category, QList<Einsatz *>*> AActivity::splitNachKategorie() const
{
    QMap<Category, QList<Einsatz*>*> gruppen;
    for(Category cat: ANZEIGEREIHENFOLGEGESAMT) {
        switch (cat) {
        case Category::Gesamt:
        case Category::Kilometer:
        case Category::Anzahl:
            continue;
        default:
            gruppen.insert(cat, new QList<Einsatz*>());
        }
    }
    gruppen.insert(Begleiter, new QList<Einsatz*>());

    for(Einsatz *e: personen) {
        switch (e->getKategorie()) {

        case Category::Gesamt:
        case Category::Kilometer:
        case Category::Anzahl:
            continue;
        default:
            gruppen.value(e->getKategorie())->append(e);
        }
    }
    return gruppen;
}

bool AActivity::hasQualification(Person *p, Category kat, QString bemerkung, QDate datum)
{
    if (p == nullptr) return false;
    if (p->isTauglich(kat, datum)) return true;

    for(const QString &s: QUALIFICATION_LIST) {
        if (bemerkung.contains(s, Qt::CaseInsensitive)) {
            return true;
        }
    }
    return false;
}

bool AActivity::isExtern(QString bemerkung)
{
    for(const QString &s: EXTERNAL_LIST) {
        if (bemerkung.contains(s, Qt::CaseInsensitive)) {
            return true;
        }
    }
    return false;
}

bool AActivity::getZeitenUnbekannt() const
{
    return zeitenUnbekannt;
}
void AActivity::setZeitenUnbekannt(bool value)
{
    zeitenUnbekannt = value;
    emit changed(this);
}

void AActivity::sort(QList<AActivity *> *list)
{
    std::sort(list->begin(), list->end(), AActivity::lesser);
}

bool AActivity::check(Auswahl aus) const
{
    if (getVon() < aus.getAb())
        return false;

    if (getBis() > aus.getBis())
        return false;

    // Auch Aktivitaeten?
    if (!aus.getActivities()) {
        if (getArt() == Art::Arbeitseinsatz)
            return false;
    }
    return true;
}

QList<Einsatz*> AActivity::getPersonen() const
{
    return personen;
}

QString AActivity::getStringShort() const
{
    QString s = "";
    if (art == Arbeitseinsatz && anlass != "")
        s = anlass;
    else
        s = toString(art);
    return s + (abgesagt ? " (Abg.)" : "")
            + (wichtig ? "!!" : "");
}

QString AActivity::getString() const
{
    QString s = datum.toString(QObject::tr("dddd dd.MM.yyyy"))+" – ";
    if (art == Arbeitseinsatz && anlass != "")
        s += anlass;
    else
        s += toString(art);
    if (abgesagt) {
        s += " (Abgesagt)";
    }
    if (wichtig) {
        s += " WICHTIG!";
    }
    return s;
}

QString AActivity::getHtmlForSingleView() const
{
    QString required = "<font color='"+COLOR_REQUIRED+"'>%1</font>";
    QString html = "";
    // Überschrift
    html += "<h2 class='pb'>";
    if (anlass != "")
        html += anlass;
    else
        html += "Arbeitseinsatz";
    html += " am " + datum.toString("dddd, dd.MM.yyyy");
    if (abgesagt)
        html += required.arg(" ABGESAGT!");
    if (wichtig)
        html += required.arg(" WICHTIG!");
    html += "</h2>";
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
        html += "<p><b>Zeiten</b>:<br/>Beginn: "+getZeitAnfang().toString("hh:mm")+"<br/>";
        if (datum < QDate::currentDate()) {
            html += "Ende: "+getZeitEnde().toString("hh:mm")+"</p>";
        } else {
            html += "Geplantes Ende: "+getZeitEnde().toString("hh:mm")+"</p>";
        }
    }
    // Personal
    html += "<p><b>Helfer";
    html += (personalBenoetigt ? required.arg(" benötigt"):"");
    html += ":</b></p>";
    if (personen.count() > 0) {
        html += "<table cellspacing='0' width='100%'><thead><tr><th>Name</th><th>Beginn*</th><th>Ende*</th><th>Aufgabe</th></tr></thead><tbody>";
        for(Einsatz *e: personen) {
            html += "<tr><td>"+e->getPerson()->getName()+"</td><td>";
            html += (e->getBeginnAbweichend() == QTime(0,0) ? "" : e->getBeginnAbweichend().toString("hh:mm"));
            html += "</td><td>";
            html += (e->getEndeAbweichend() == QTime(0,0) ? "" : e->getEndeAbweichend().toString("hh:mm"));
            html += "</td><td>";
            if (e->getKategorie() != Category::Sonstiges) {
                html += toString(e->getKategorie());
                if (e->getBemerkung() != "") html += "<br/>";
            }
            html += e->getBemerkung();
            html +="</td></tr>";
        }
        html += "</tbody></table><p>* Abweichend von obigen Zeiten!</p>";
    }
    return html;
}

QString AActivity::getHtmlForTableView() const
{
    QString html = "<tr bgcolor='"+getFarbe()+"'>";
    // Datum, Anlass
    if (wichtig) {
        html += "<td bgcolor='#ff8888'>";
    } else {
        html += "<td>";
    }
    html += "<b>"+datum.toString("dddd d.M.yyyy")+"</b><br</>";
    if (anlass != "") {
        html += anlass;
    } else {
        html += "Arbeitseinsatz";
    }
    if (ort != "") {
        html += " in "+ort;
    }
    html += "</td>";

    if (abgesagt) {
        html += "<td colspan='4'><b>Abgesagt!</b></td>";
        html += "<td>"+QString(bemerkungen).replace("\n", "<br/>")+"</td></tr>";
        return html;
    }

    // Dienstzeiten
    if (zeitenUnbekannt) {
        html += "<td>Zeiten werden noch bekannt gegeben!</td>";
    } else {
        html += "<td>Beginn: "+getZeitAnfang().toString("hh:mm") + "<br/>";
        if (datum < QDate::currentDate()) {
            html += "Ende: "+getZeitEnde().toString("hh:mm") + "</td>";
        } else {
            html += "Ende: ~"+getZeitEnde().toString("hh:mm") + "</td>";
        }
    }

    QMap<Category, QList<Einsatz*>*> gruppen = splitNachKategorie();

    // Tf, Tb
    html += "<td>";
    if (gruppen.value(Tf)->size() + gruppen.value(Tb)->size() > 0) {
        html += "<ul>" + listToString("", *gruppen.value(Tf), "<li>", "</li>");
        html += listToString("", *gruppen.value(Tb), "<li><i>", "</i></li>") + "</ul>";
        gruppen.remove(Tf);
        gruppen.remove(Tb);
    }
    html += "</td>";

    // Zf, Zub, Begl.o.b.A.
    html += "<td><ul>";
    if (gruppen.value(Zf)->size() > 0) {
        html += listToString("", *gruppen.value(Zf), "<li><u>", "</u></li>");
        gruppen.remove(Zf);
    }
    if (gruppen.value(Zub)->size() > 0) {
        html += listToString("", *gruppen.value(Zub), "<li>", "</li>");
        gruppen.remove(Zub);
    }
    if (gruppen.value(Begleiter)->size() > 0) {
        html += listToString("", *gruppen.value(Begleiter), "<li><i>", "</i></li>");
        gruppen.remove(Begleiter);
    }
    html += "</ul></td>";

    // Service
    html += "<td>";
    if (gruppen.value(Service)->size() > 0) {
        html += "<ul>" + listToString("", *gruppen.value(Service), "<li>", "</li>") + "</ul>";
        gruppen.remove(Service);
    }
    html += "</td>";

    // Sonstiges
    bool zeilenUmbruch = false;
    if (personalBenoetigt) {
        if (QDate::currentDate().addDays(10) >= datum && datum >= QDate::currentDate()) {
            html += "<td bgcolor='#ff8888'>";
        } else {
            html += "<td>";
        }
        html += "<b>Helfer benötigt!</b>";
        zeilenUmbruch = true;
    } else {
        html += "<td>";
    }

    if (gruppen.value(Werkstatt)->size() >= 2) {
        if (zeilenUmbruch) html += "<br/>";
        zeilenUmbruch = false;
        html += "<b>Werkstatt:</b><ul>";
        html += listToString("", *gruppen.value(Werkstatt), "<li>", "</li>");
        html += "</ul>";
    } else {
        gruppen.value(Sonstiges)->append(*gruppen.value(Werkstatt));
    }
    gruppen.remove(Werkstatt);
    if (gruppen.value(Ausbildung)->size() >= 2) {
        if (zeilenUmbruch) html += "<br/>";
        zeilenUmbruch = false;
        html += "<b>Ausbildung:</b><ul>";
        html += listToString("", *gruppen.value(Ausbildung), "<li>", "</li>");
        html += "</ul>";
    } else {
        gruppen.value(Sonstiges)->append(*gruppen.value(Ausbildung));
    }
    gruppen.remove(Ausbildung);
    if (gruppen.value(ZugVorbereiten)->size() >= 2) {
        if (zeilenUmbruch) html += "<br/>";
        zeilenUmbruch = false;
        html += "<b>Zugvorbereitung:</b><ul>";
        html += listToString("", *gruppen.value(ZugVorbereiten), "<li>", "</li>");
        html += "</ul>";
    } else {
        gruppen.value(Sonstiges)->append(*gruppen.value(ZugVorbereiten));
    }
    gruppen.remove(ZugVorbereiten);
    if (gruppen.value(Infrastruktur)->size() >= 2) {
        if (zeilenUmbruch) html += "<br/>";
        zeilenUmbruch = false;
        html += "<b>Infrastruktur:</b><ul>";
        html += listToString("", *gruppen.value(Infrastruktur), "<li>", "</li>");
        html += "</ul>";
    } else {
        gruppen.value(Sonstiges)->append(*gruppen.value(Infrastruktur));
    }
    gruppen.remove(Infrastruktur);
    if (gruppen.value(Buero)->size() >= 2) {
        if (zeilenUmbruch) html += "<br/>";
        zeilenUmbruch = false;
        html += "<b>Büro:</b><ul>";
        html += listToString("", *gruppen.value(Buero), "<li>", "</li>");
        html += "</ul>";
    } else {
        gruppen.value(Sonstiges)->append(*gruppen.value(Buero));
    }
    gruppen.remove(Buero);

    for(Category cat: gruppen.keys()) {
        if (cat != Sonstiges)
            gruppen.value(Sonstiges)->append(*gruppen.value(cat));
    }
    if (! gruppen.value(Sonstiges)->isEmpty()) {
        if (html.endsWith("</ul>")) {
            if (zeilenUmbruch) html += "<br/>";
            html += "<b>Sonstiges:</b><ul>";
        } else {
            html += "<ul>";
        }
        html += listToString("", *gruppen.value(Sonstiges), "<li>", "</li>", true);
        html += "</ul>";
        zeilenUmbruch = false;
    }

    // Bemerkungen
    if (bemerkungen != "") {
        if (zeilenUmbruch) html += "<br/>";
        html += QString(bemerkungen).replace("\n", "<br/>");
    }
    html += "</td></tr>";
    return html;
}

bool AActivity::print(QPrinter *printer)
{
    return Export::druckeHtmlAufDrucker(getHtmlForSingleView() + Export::zeitStempel(false), printer);
}

QString AActivity::getFarbe() const
{
    if (anlass.contains("vlexx", Qt::CaseSensitivity::CaseInsensitive)) {
        return "#DCF57E";//a3c526'>";
    }
    switch (art) {
    case Museumszug:
        return "#ffffff";
    case Sonderzug:
        return "#ffcccc";
    case Gesellschaftssonderzug:
        return "#FFDA91"; //ffcc66
    case Nikolauszug:
        return "#ffccff";
    case ELFundMuseumszug:
        return "#e7e7fd";
    case Schnupperkurs:
        return "#918fe3";
    case Bahnhofsfest:
        return "#80e3b1";
    case SonstigerFahrtag:
        return "#ffeb90";
    case Arbeitseinsatz:
        return "#CCBEBE";
    }
}
