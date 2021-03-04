#include "aactivity.h"
#include "person.h"
#include "eplexception.h"

#include <QJsonArray>
#include <QLinkedList>

QStringList AActivity::EXTERNAL_LIST = QStringList() << "Extern" << "Führerstand" << "FS" << "Schnupperkurs" << "ELF" << "Ehrenlokführer" << "ELF-Kurs";
QStringList AActivity::QUALIFICATION_LIST = QStringList() << "Azubi" << "Ausbildung" << "Tf-Ausbildung" << "Zf-Ausbildung" << "Tf-Unterricht" << "Zf-Unterricht" << "Weiterbildung";
QString AActivity::COLOR_REQUIRED = "#ff3333";

const QString AActivity::KOPF_LISTE_HTML = "<h3>Übersicht über die Aktivitäten</h3>"
                                         "<table cellspacing='0' width='100%'><thead><tr>"
                                         "<th>Datum, Anlass</th> <th>Dienstzeiten</th>"
                                         "<th>Tf, Tb</th> <th><u>Zf</u>, Zub, <i>Begl.o.b.A</i></th> <th>Service</th>"
                                         "<th>Sonstiges</th> </tr></thead><tbody>";
const QString AActivity::FUSS_LISTE_HTML = "</tbody></table>";

AActivity::AActivity(QDate date, ManagerPersonal *p) : QObject()
{
    art = Arbeitseinsatz;
    datum = date;
    ort = "";
    zeitAnfang = QTime(10, 0);
    zeitEnde = QTime(16, 0);
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
    zeitAnfang = QTime::fromString(o.value("zeitAnfang").toString(), "hh:mm");
    zeitEnde = QTime::fromString(o.value("zeitEnde").toString(), "hh:mm");
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

        Einsatz *e = new Einsatz{person, this,
                      static_cast<Category>(aO.value("kat").toInt(100)),
                      QTime::fromString(aO.value("beginn").toString(), "hh:mm"),
                      QTime::fromString(aO.value("ende").toString(), "hh:mm"),
                      aO.value("bemerkung").toString()};

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
        (e->person)->removeActivity(e);
    }
}

Art AActivity::getArt() const
{
    return Art::Arbeitseinsatz;
}

QJsonObject AActivity::toJson()
{
    QJsonObject data;
    data.insert("art", static_cast<int>(Art::Arbeitseinsatz));
    data.insert("datum", datum.toString("yyyy-MM-dd"));
    data.insert("ort", ort);
    data.insert("zeitAnfang", zeitAnfang.toString("hh:mm"));
    data.insert("zeitEnde", zeitEnde.toString("hh:mm"));
    data.insert("zeitenUnbekannt", zeitenUnbekannt);
    data.insert("anlass", anlass);
    data.insert("bemerkungen", bemerkungen);
    QJsonArray personenJSON;
    for(Einsatz *e: personen) {
        QJsonObject persJson;
        if (personal->personExists((e->person)->getName())) {
            persJson.insert("id", (e->person)->getId());
        } else {
            persJson.insert("name", (e->person)->getName());
        }
        persJson.insert("beginn", e->beginn.toString("hh:mm"));
        persJson.insert("ende", e->ende.toString("hh:mm"));
        persJson.insert("kat", e->kategorie);
        persJson.insert("bemerkung", e->bemerkung);
        personenJSON.append(persJson);
    }
    data.insert("personen", personenJSON);
    data.insert("personalBenoetigt", personalBenoetigt);
    data.insert("wichtig", wichtig);
    data.insert("abgesagt", abgesagt);
    return data;
}

QDate AActivity::getDatum()
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

QTime AActivity::getZeitAnfang()
{
    return zeitAnfang;
}
void AActivity::setZeitAnfang(QTime value)
{
    zeitAnfang = value;
    emit changed(this);
}

QTime AActivity::getZeitEnde()
{
    return zeitEnde;
}
void AActivity::setZeitEnde(QTime value)
{
    zeitEnde = value;
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
    if (e->person != nullptr) {
        ret = e->person->removeActivity(e);
    }
    emit changed(this);
    return ret;
}

Einsatz *AActivity::addPerson(Person *p, QString bemerkung, QTime start, QTime ende, Category kat)
{
   /*
    * Angaben fuer die Aufgabe pruefen
    * pruefen ob die Person geeignet ist
    * Person hinzufuegen oder Fehlermeldung zurueckgeben
    */
    if (p == nullptr)
        throw AActivityException();

    if (kat == Zub && !hasQualification(p, kat, bemerkung, datum)) {
        kat = Begleiter;
    }

    if (! hasQualification(p, kat, bemerkung, datum))
        throw FalscheQualifikationException(p->getName());

    // jetzt ist alles richtig und die person kann registiert werden.

    Einsatz *e = new Einsatz{p, this, kat, start, ende, bemerkung};

    p->addActivity(e);
    personen.append(e);

    emit changed(this);
    return e;
}
Einsatz *AActivity::addPerson(QString p, QString bemerkung, QTime start, QTime ende, Category kat)
{
    Person *pers = personal->getPerson(p);
    if (pers != nullptr) {
        return addPerson(pers, bemerkung, start, ende, kat);
    }
    if (isExtern(bemerkung)) {
        if (kat == Category::Zub) kat = Category::Begleiter;
        Person *neuePerson = new Person(p, nullptr);
        neuePerson->setAusbildungTf(true);
        neuePerson->setAusbildungZf(true);
        neuePerson->setAusbildungRangierer(true);
        return addPerson(neuePerson, bemerkung, start, ende, kat);
    }
    throw PersonNichtGefundenException(p);
}

bool AActivity::lesser(const AActivity *lhs, const AActivity *rhs)
{
    // Datum
    if (lhs->datum < rhs->datum)
        return true;
    if (lhs->datum > rhs->datum)
        return false;
    // Zeiten?
    if (lhs->zeitenUnbekannt && !rhs->zeitenUnbekannt)
        return true;
    if (!lhs->zeitenUnbekannt && rhs->zeitenUnbekannt)
        return false;

    if (!lhs->zeitenUnbekannt) {
        // Beginn
        if (lhs->zeitAnfang < rhs->zeitAnfang)
            return true;
        if (lhs->zeitAnfang > rhs->zeitAnfang)
            return false;
        // Ende
        if (lhs->zeitEnde < rhs->zeitEnde)
            return true;
        if (lhs->zeitEnde > rhs->zeitEnde)
            return false;
    }
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

ManagerPersonal *AActivity::getPersonal() const
{
    return personal;
}

QString AActivity::listToString(QString sep, QList<Einsatz*> liste, QString prefix, QString suffix, bool aufgabe)
{
    QStringList l;
    QStringList l2;
    for(Einsatz *e: liste) {
        l2.append(e->person->getName());

        if (e->bemerkung != "") {
            l2.append(e->bemerkung);
        }
        if (aufgabe && (e->kategorie != Category::Sonstiges)) {
            l2.append(getLocalizedStringFromCategory(e->kategorie));
        }
        l.append(prefix+l2.join("; ")+suffix);
        l2.clear();
    }
    return l.join(sep);
}

bool AActivity::hasQualification(Person *p, Category kat, QString bemerkung, QDate datum)
{
    if (p == nullptr) return false;
    if (p->isTauglich(kat, datum)) return true;

    for(QString s: QUALIFICATION_LIST) {
        if (bemerkung.contains(s, Qt::CaseInsensitive)) {
            return true;
        }
    }
    return false;
}

bool AActivity::isExtern(QString bemerkung)
{
    for(QString s: EXTERNAL_LIST) {
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

bool AActivity::liegtInVergangenheit()
{
    if (datum < QDate::currentDate()) return true;
    if (datum > QDate::currentDate()) return false;
    if (zeitEnde <= QTime::currentTime()) return true;
    return false;
}

void AActivity::sort(QList<AActivity *> *list)
{
    std::sort(list->begin(), list->end(), AActivity::lesser);
}

bool AActivity::check(Auswahl aus)
{
    switch (aus.startdate) {
    case Auswahl::AbJetzt:
        if (liegtInVergangenheit()) {
            return false;
        }
        break;
    case Auswahl::AbAlle:
        break;
    case Auswahl::AbAnfangDesJahres:
        if (getDatum().year() < QDate::currentDate().year()) {
            return false;
        }
        break;
    default:
        break;
    }
    // Enddatum
    QDate ref;
    switch (aus.enddate) {
    case Auswahl::BisAlle:
        break;
    case Auswahl::BisEndeNaechsterWoche:
        ref = QDate::currentDate().addDays(7); // naechste Woche
        ref = ref.addDays(7-ref.dayOfWeek()); // Ende der Woche
        if (getDatum() > ref) {
            return false;
        }
        break;
    case Auswahl::BisEndeNaechsterMonat:
        ref = QDate::currentDate().addMonths(1); // naechster Monat
        ref = QDate(ref.year(), ref.month(), ref.daysInMonth()); // Ende des Monats
        if (getDatum() > ref) {
            return false;
        }
    case Auswahl::BisEndeDesJahres:
        if (getDatum().year() > QDate::currentDate().year()) {
            return false;
        }
    default:
        break;
    }
    // Auch Aktivitaeten?
    if (!aus.activities) {
        if (getArt() == Art::Arbeitseinsatz)
            return false;
    }
    return true;
}

QList<Einsatz*> AActivity::getPersonen() const
{
    return personen;
}

QString AActivity::getKurzbeschreibung()
{
    if (anlass != "")
        return anlass;
    return getStringFromArt(Arbeitseinsatz);
}

QString AActivity::getListStringShort()
{
    QString s = getStringFromArt(art);
    if (anlass != "")
        s = anlass;
    return s + (abgesagt ? " (Abg.)" : "")
            + (wichtig ? "!!" : "");
}

QString AActivity::getListString()
{
    QString s = datum.toString(QObject::tr("dddd dd.MM.yyyy"))+" – ";
    if (art == Arbeitseinsatz && anlass != "")
        s += anlass;
    else
        s += getStringFromArt(art);
    if (abgesagt) {
        s += " (Abgesagt)";
    }
    if (wichtig) {
        s += " WICHTIG!";
    }
    return s;
}

QList<Einsatz> AActivity::getIndividual(Person *person)
{
    QList<Einsatz> liste;
    if (person == nullptr)
        return liste;
    for(Einsatz *e: personen) {
        if (e->person == person) {
            Einsatz neu = Einsatz(*e);
            if (zeitenUnbekannt) {
                neu.beginn = QTime(0,0);
                neu.ende = QTime(0,0);
            } else {
                if (neu.beginn == QTime(0,0)) {
                    neu.beginn = zeitAnfang;
                }
                if (neu.ende == QTime(0,0)) {
                    neu.ende = zeitEnde;
                }
            }
            neu.anrechnen = !abgesagt;
            if (datum > QDate::currentDate()) {
                neu.anrechnen = false;
            }
            liste.append(neu);
        }
    }
    return liste;
}

QString AActivity::getHtmlForSingleView()
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
        html += "<p><b>Zeiten</b>:<br/>Beginn: "+zeitAnfang.toString("hh:mm")+"<br/>";
        if (datum < QDate::currentDate()) {
            html += "Ende: "+zeitEnde.toString("hh:mm")+"</p>";
        } else {
            html += "Geplantes Ende: "+zeitEnde.toString("hh:mm")+"</p>";
        }
    }
    // Personal
    html += "<p><b>Helfer";
    html += (personalBenoetigt ? required.arg(" benötigt"):"");
    html += ":</b></p>";
    if (personen.count() > 0) {
        html += "<table cellspacing='0' width='100%'><thead><tr><th>Name</th><th>Beginn*</th><th>Ende*</th><th>Aufgabe</th></tr></thead><tbody>";
        for(Einsatz *e: personen) {
            html += "<tr><td>"+e->person->getName()+"</td><td>";
            html += (e->beginn == QTime(0,0) ? "" : e->beginn.toString("hh:mm"));
            html += "</td><td>";
            html += (e->ende == QTime(0,0) ? "" : e->ende.toString("hh:mm"));
            html += "</td><td>";
            if (e->kategorie != Category::Sonstiges) {
                html += getLocalizedStringFromCategory(e->kategorie);
                if (e->bemerkung != "") html += "<br/>";
            }
            html += e->bemerkung;
            html +="</td></tr>";
        }
        html += "</tbody></table><p>* Abweichend von obigen Zeiten!</p>";
    }
    return html;
}

QString AActivity::getHtmlForTableView()
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
        html += "<td>"+bemerkungen.replace("\n", "<br/>")+"</td></tr>";
        return html;
    }

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

    QList<Einsatz*> tf;
    QList<Einsatz*> zf;
    QList<Einsatz*> zub;
    QList<Einsatz*> begl;
    QList<Einsatz*> service;
    QList<Einsatz*> sonstige;
    QList<Einsatz*> werkstatt;
    QList<Einsatz*> zugvorbereitung;
    QList<Einsatz*> ausbildung;
    QList<Einsatz*> infrastruktur;

    // Aufsplitten der Personen auf die Einzelnen Listen
    for(Einsatz *e: personen) {
        switch (e->kategorie) {
        case Category::Tf:
        case Category::Tb: tf.append(e); break;
        case Category::Zf: zf.append(e); break;
        case Category::Service: service.append(e); break;
        case Category::Zub: zub.append(e); break;
        case Category::Begleiter: begl.append(e); break;
        case Category::Werkstatt: werkstatt.append(e); break;
        case Category::ZugVorbereiten: zugvorbereitung.append(e); break;
        case Category::Ausbildung: ausbildung.append(e); break;
        case Category::Infrastruktur: infrastruktur.append(e); break;
        case Category::Sonstiges: sonstige.append(e); break;
        case Category::Buero:
        case Category::Gesamt:
        case Category::Kilometer:
        case Category::Anzahl:
            continue;
        }
    }

    // Tf, Tb
    html += "<td>";
    if (tf.size() > 0) {
        html += "<ul>" + listToString("", tf, "<li>", "</li>") + "</ul>";
    }
    html += "</td>";

    // Zf, Zub, Begl.o.b.A.
    html += "<td><ul>";
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
    html += "<td>";
    if (service.size() > 0) {
        html += "<ul>" + listToString("", service, "<li>", "</li>") + "</ul>";
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

    if (werkstatt.size() >= 2) {
        if (zeilenUmbruch) html += "<br/>";
        zeilenUmbruch = false;
        html += "<b>Werkstatt:</b><ul>";
        html += listToString("", werkstatt, "<li>", "</li>");
        html += "</ul>";
    } else {
        sonstige.append(werkstatt);
        werkstatt.clear();
    }
    if (ausbildung.size() >= 2) {
        if (zeilenUmbruch) html += "<br/>";
        zeilenUmbruch = false;
        html += "<b>Ausbildung:</b><ul>";
        html += listToString("", ausbildung, "<li>", "</li>");
        html += "</ul>";
    } else {
        sonstige.append(ausbildung);
        ausbildung.clear();
    }
    if (zugvorbereitung.size() >= 2) {
        if (zeilenUmbruch) html += "<br/>";
        zeilenUmbruch = false;
        html += "<b>Zugvorbereitung:</b><ul>";
        html += listToString("", zugvorbereitung, "<li>", "</li>");
        html += "</ul>";
    } else {
        sonstige.append(zugvorbereitung);
        zugvorbereitung.clear();
    }
    if (infrastruktur.size() >= 2) {
        if (zeilenUmbruch) html += "<br/>";
        zeilenUmbruch = false;
        html += "<b>Infrastruktur:</b><ul>";
        html += listToString("", infrastruktur, "<li>", "</li>");
        html += "</ul>";
    } else {
        sonstige.append(infrastruktur);
        infrastruktur.clear();
    }
    if (sonstige.size() > 0) {
        if (werkstatt.size() + ausbildung.size() + zugvorbereitung.size() + infrastruktur.size() > 0) {
            if (zeilenUmbruch) html += "<br/>";
            html += "<b>Sonstiges:</b><ul>";
        } else {
            html += "<ul>";
        }
        html += listToString("", sonstige, "<li>", "</li>", true);
        html += "</ul>";
        zeilenUmbruch = false;
    }

    // Bemerkungen
    if (bemerkungen != "") {
        if (zeilenUmbruch) html += "<br/>";
        html += bemerkungen.replace("\n", "<br/>");
    }
    html += "</td></tr>";
    return html;
}

QString AActivity::getFarbe()
{
    if (anlass.contains("vlexx", Qt::CaseSensitivity::CaseInsensitive)) {
        return "#DCF57E";//a3c526'>";
    }
    return FARBE_FAHRTAGE.value(art);
}
