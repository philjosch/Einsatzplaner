#include "aactivity.h"
#include "person.h"
#include "eplexception.h"

#include <QJsonArray>
#include <QLinkedList>

using namespace EplException;

const QStringList AActivity::EXTERNAL_LIST = QStringList() << "Extern" << "Führerstand" << "FS" << "Schnupperkurs" << "ELF" << "Ehrenlokführer" << "ELF-Kurs";
const QStringList AActivity::QUALIFICATION_LIST = QStringList() << "Azubi" << "Ausbildung" << "Tf-Ausbildung" << "Zf-Ausbildung" << "Tf-Unterricht" << "Zf-Unterricht" << "Weiterbildung";
const QString AActivity::COLOR_REQUIRED = "#ff3333";

const QString AActivity::KOPF_LISTE_HTML = "<h3>Übersicht über die Aktivitäten</h3>"
                                         "<table cellspacing='0' width='100%'><thead><tr>"
                                         "<th>Datum, Anlass</th> <th>Dienstzeiten</th>"
                                         "<th>Tf, Tb</th> <th><u>Zf</u>, Zub, <i>Begl.o.b.A</i></th> <th>Service</th>"
                                         "<th>Sonstiges</th> </tr></thead><tbody>";
const QString AActivity::FUSS_LISTE_HTML = "</tbody></table>";

const QMap<Art, QString> AActivity::FARBE_FAHRTAGE = {{Museumszug, "#ffffff"},
                                          {Sonderzug, "#ffcccc"},
                                          {Gesellschaftssonderzug, "#FFDA91"},//ffcc66"},
                                          {Nikolauszug, "#ffccff"},
                                          {ELFundMuseumszug, "#e7e7fd"},
                                          {Schnupperkurs, "#918fe3"},
                                          {Bahnhofsfest, "#80e3b1"},
                                          {SonstigerFahrtag, "#ffeb90"},
                                          {Arbeitseinsatz, "#CCBEBE"}
                                         };

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
        if (personal->personExists(e->getPerson()->getName())) {
            persJson.insert("id", e->getPerson()->getId());
        } else {
            persJson.insert("name", e->getPerson()->getName());
        }
        persJson.insert("beginn", e->getBeginnFiktiv().toString("hh:mm"));
        persJson.insert("ende", e->getEndeFiktiv().toString("hh:mm"));
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

QTime AActivity::getAnfang(const Category kat) const
{
    if (abgesagt)
        return QTime();
    if (zeitenUnbekannt)
        return QTime();
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

QTime AActivity::getEnde(const Category kat) const
{
    if (abgesagt)
        return QTime();
    if (zeitenUnbekannt)
        return QTime();
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

    if (kat == Tf && bemerkung.contains(getLocalizedStringFromCategory(Tb), Qt::CaseInsensitive))
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

QString AActivity::listToString(QString sep, QList<Einsatz*> liste, QString prefix, QString suffix, bool aufgabe)
{
    QStringList l;
    QStringList l2;
    for(Einsatz *e: liste) {
        l2.append(e->getPerson()->getName());

        if (e->getBemerkung() != "") {
            l2.append(e->getBemerkung());
        }
        if (aufgabe && (e->getKategorie() != Category::Sonstiges)) {
            l2.append(getLocalizedStringFromCategory(e->getKategorie()));
        }
        l.append(prefix+l2.join("; ")+suffix);
        l2.clear();
    }
    return l.join(sep);
}

QDateTime AActivity::getAnfangGenau()
{
    if (zeitenUnbekannt)
        return QDateTime(datum, QTime(0, 0));
    else
        return QDateTime(datum, zeitAnfang);
}

QDateTime AActivity::getEndeGenau()
{
    if (zeitenUnbekannt)
        return QDateTime(datum, QTime(23, 59, 59, 999));
    else
        return QDateTime(datum, zeitEnde);
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
    if (getAnfangGenau() < aus.getAb())
        return false;

    if (getEndeGenau() > aus.getBis())
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
            html += "<tr><td>"+e->getPerson()->getName()+"</td><td>";
            html += (e->getBeginnFiktiv() == QTime(0,0) ? "" : e->getBeginnFiktiv().toString("hh:mm"));
            html += "</td><td>";
            html += (e->getEndeFiktiv() == QTime(0,0) ? "" : e->getEndeFiktiv().toString("hh:mm"));
            html += "</td><td>";
            if (e->getKategorie() != Category::Sonstiges) {
                html += getLocalizedStringFromCategory(e->getKategorie());
                if (e->getBemerkung() != "") html += "<br/>";
            }
            html += e->getBemerkung();
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
        switch (e->getKategorie()) {
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

QString AActivity::getFarbe() const
{
    if (anlass.contains("vlexx", Qt::CaseSensitivity::CaseInsensitive)) {
        return "#DCF57E";//a3c526'>";
    }
    return FARBE_FAHRTAGE.value(art);
}
