#include "aactivity.h"
#include "person.h"
#include "fahrtag.h"
#include "activity.h"

#include <QJsonArray>
#include <QLinkedList>

QStringList AActivity::EXTERNAL_LIST = QStringList() << "Extern" << "Führerstand" << "FS" << "Schnupperkurs" << "ELF" << "Ehrenlokführer" << "ELF-Kurs";
QStringList AActivity::QUALIFICATION_LIST = QStringList() << "Azubi" << "Ausbildung" << "Tf-Ausbildung" << "Zf-Ausbildung" << "Tf-Unterricht" << "Zf-Unterricht" << "Weiterbildung";
QString AActivity::COLOR_REQUIRED = "#ff3333";

const QString getFarbe(AActivity *a)
{
    if (Fahrtag *f = dynamic_cast<Fahrtag*>(a))
        return FARBE_FAHRTAGE.value(f->getArt());
    else
        return FARBE_FAHRTAGE.value(Arbeitseinsatz);
}

AActivity::AActivity(QDate date, ManagerPersonal *p) : QObject()
{
    datum = date;
    ort = "";
    zeitAnfang = QTime(10, 0);
    zeitEnde = QTime(16, 0);
    zeitenUnbekannt = false;
    anlass = "";
    bemerkungen = "";
    personen = QMap<Person *, Infos>();
    personalBenoetigt = true;
    personal = p;
}

AActivity::AActivity(QJsonObject o, ManagerPersonal *p) : QObject()
{
    personal = p;
    personen = QMap<Person*, Infos>();

    datum = QDate::fromString(o.value("datum").toString(), "yyyy-MM-dd");
    ort = o.value("ort").toString();
    zeitAnfang = QTime::fromString(o.value("zeitAnfang").toString(), "hh:mm");
    zeitEnde = QTime::fromString(o.value("zeitEnde").toString(), "hh:mm");
    zeitenUnbekannt = o.value("zeitenUnbekannt").toBool();
    anlass = o.value("anlass").toString().replace("<br/>","\n");
    bemerkungen = o.value("bemerkungen").toString().replace("<br/>", "\n");
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

        Infos info = Infos();
        info.beginn = QTime::fromString(aO.value("beginn").toString(), "hh:mm");
        info.ende = QTime::fromString(aO.value("ende").toString(), "hh:mm");
        info.kategorie = static_cast<Category>(aO.value("kat").toInt(100));
        info.bemerkung = aO.value("bemerkung").toString();

        person->addActivity(this, info.kategorie);
        personen.insert(person, info);
    }
    personalBenoetigt = o.value("personalBenoetigt").toBool(true);
}

AActivity::~AActivity()
{
    remove();
}

bool AActivity::remove()
{
    for(Person *p: personen.keys()) {
        p->removeActivity(this);
    }
    return true;
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
    for(Person *p: personen.keys()) {
        QJsonObject persJson;
        if (personal->personExists(p->getName())) {
            persJson.insert("id", p->getId());
        } else {
            persJson.insert("name", p->getName());
        }
        persJson.insert("beginn", personen.value(p).beginn.toString("hh:mm"));
        persJson.insert("ende", personen.value(p).ende.toString("hh:mm"));
        persJson.insert("kat", personen.value(p).kategorie);
        persJson.insert("bemerkung", personen.value(p).bemerkung);
        personenJSON.append(persJson);
    }
    data.insert("personen", personenJSON);
    data.insert("personalBenoetigt", personalBenoetigt);
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

QMap<Person *, Infos> AActivity::getPersonen()
{
    return personen;
}

bool AActivity::removePerson(Person *p)
{
    personen.remove(p);
    emit changed(this);
    return p->removeActivity(this);
}

bool AActivity::removePerson(QString p)
{
    Person *pers = personal->getPerson(p);
    if (pers != nullptr) {
        emit changed(this);
        return removePerson(pers);
    }
    Person *gefunden = nullptr;
    for(Person *pers: personen.keys()) {
        if (pers->getName() == p) {
            gefunden = pers;
            break;
        }
    }
    if (gefunden != nullptr) {
        personen.remove(gefunden);
        emit changed(this);
        return true;
    }
    return false;
}

Mistake AActivity::addPerson(Person *p, QString bemerkung, QTime start, QTime ende, Category kat)
{
   /*
    * Angaben fuer die Aufgabe pruefen
    * pruefen ob die Person geeignet ist
    * Person hinzufuegen oder Fehlermeldung zurueckgeben
    */

    if (kat == Zub && !hasQualification(p, kat, bemerkung)) kat = Begleiter;
    if (kat == Tf && bemerkung.contains(QObject::tr("Tb"),Qt::CaseInsensitive)) kat = Tb;

    if (! hasQualification(p, kat, bemerkung, datum)) return Mistake::FalscheQualifikation;

    // jetzt ist alles richtig und die person kann registiert werden.
    p->addActivity(this, kat);

    Infos info = Infos();
    info.beginn = start;
    info.ende = ende;
    info.kategorie = kat;
    info.bemerkung = bemerkung;

    personen.insert(p, info);

    emit changed(this);
    return Mistake::OK;
}

Mistake AActivity::addPerson(QString p, QString bemerkung, QTime start, QTime ende, Category kat)
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
        Mistake atw = addPerson(neuePerson, bemerkung, start, ende, kat);
        if (atw == Mistake::OK) return ExternOk;
        return atw;
    }
    return Mistake::PersonNichtGefunden;
}

void AActivity::updatePersonBemerkung(Person *p, QString bem)
{
    Infos i = personen.value(p);
    i.bemerkung = bem;
    personen.insert(p, i);
}

bool AActivity::lesser(const AActivity &second) const
{
    // Datum
    if (this->datum < second.datum)
        return true;
    if (this->datum > second.datum)
        return false;
    // Zeiten?
    if (this->zeitenUnbekannt && !second.zeitenUnbekannt)
        return true;
    if (!this->zeitenUnbekannt && second.zeitenUnbekannt)
        return false;

    if (!this->zeitenUnbekannt) {
        // Beginn
        if (this->zeitAnfang < second.zeitAnfang)
            return true;
        if (this->zeitAnfang > second.zeitAnfang)
            return false;
        // Ende
        if (this->zeitEnde < second.zeitEnde)
            return true;
        if (this->zeitEnde > second.zeitEnde)
            return false;
    }
    // Art und beliebig, bei gleicher Art
    if (const Fahrtag *f = dynamic_cast<const Fahrtag*>(this))
        return true;
    if (const Activity *a = dynamic_cast<const Activity*>(&second))
        return true;
    return false;
}

ManagerPersonal *AActivity::getPersonal() const
{
    return personal;
}

QString AActivity::listToString(QMap<Person *, Infos> liste, QString seperator, bool aufgabe)
{
    QString a = "";
    for(Person *p: liste.keys()) {
        a+= p->getName();
        bool strichPunkt = false;
        if (liste.value(p).bemerkung!= "") {
            a += "; "+liste.value(p).bemerkung;
            strichPunkt = true;
        }
        if (aufgabe && (liste.value(p).kategorie != Category::Sonstiges)) {
            if (strichPunkt) {
                a += ", ";
            } else {
                a += "; ";
                strichPunkt = true;
            }
            a += getLocalizedStringFromCategory(liste.value(p).kategorie);
        }
        if (p != liste.keys().last()) {
            a += seperator;
        }
    }
    return a;
}

bool AActivity::hasQualification(Person *p, Category kat, QString bemerkung, QDate datum)
{
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

QComboBox *AActivity::generateNewCategoryComboBox()
{
    QComboBox *box = new QComboBox();
    box->insertItems(0, QStringList({getLocalizedStringFromCategory(Tf),
                                     getLocalizedStringFromCategory(Tb),
                                     getLocalizedStringFromCategory(Zf),
                                     getLocalizedStringFromCategory(Service),
                                     getLocalizedStringFromCategory(Zub),
                                     getLocalizedStringFromCategory(Buero),
                                     getLocalizedStringFromCategory(Werkstatt),
                                     getLocalizedStringFromCategory(ZugVorbereiten),
                                     getLocalizedStringFromCategory(Ausbildung),
                                     getLocalizedStringFromCategory(Infrastruktur),
                                     getLocalizedStringFromCategory(Sonstiges)}));
    box->setCurrentIndex(10);
    return box;
}

QTimeEdit *AActivity::generateNewTimeEdit()
{
    QTimeEdit *edit = new QTimeEdit();
    edit->setDisplayFormat("hh:mm");
    return edit;
}

void AActivity::sort(QList<AActivity *> *list)
{
    AActivity::mergeSort(list, 0, list->length()-1);
}

void AActivity::merge(QList<AActivity*> *arr, int l, int m, int r)
{
    // First subarray is arr[l..m]
    // Second subarray is arr[m+1..r]
    QLinkedList<AActivity*> L, R = QLinkedList<AActivity*>();

    for (int i = l; i <= m; i++)
        L.append(arr->at(i));
    for (int j = m+1; j <= r; j++)
        R.append(arr->at(j));

    /* Merge the temp arrays back into arr[l..r]*/
    int i = l;
    while ((!(L.isEmpty())) && (!(R.isEmpty()))) {
        if ( *(L.first()) <= *(R.first()) ) {
            arr->replace(i++, L.first());
            L.removeFirst();
        } else {
            arr->replace(i++, R.first());
            R.removeFirst();
        }
    }
    while(! L.isEmpty()) {
        arr->replace(i++, L.first());
        L.removeFirst();
    }
    while (! R.isEmpty()) {
        arr->replace(i++, R.first());
        R.removeFirst();
    }
}

/* l is for left index and r is right index of the sub-array of arr to be sorted */
void AActivity::mergeSort(QList<AActivity*> *arr, int l, int r)
{
    if (l < r)
    {
        // Same as (l+r)/2, but avoids overflow for
        // large l and h
        int m = l+(r-l)/2;

        // Sort first and second halves
        mergeSort(arr, l, m);
        mergeSort(arr, m+1, r);

        merge(arr, l, m, r);
    }
}

QString AActivity::getKurzbeschreibung()
{
    if (anlass != "")
        return anlass;
    return "Arbeitseinsatz";
}

QString AActivity::getListStringShort() {
    if (anlass != "")
        return anlass;
    if (bemerkungen != "")
        return bemerkungen;
    return "Arbeitseinsatz";
}

QString AActivity::getListString()
{
    QString scnd = tr("Arbeitseinsatz");
    if (anlass != "")
        scnd = anlass;
    return datum.toString("dddd dd.MM.yyyy")+" – "+scnd;
}

Infos AActivity::getIndividual(Person *person)
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

QString AActivity::getHtmlForSingleView()
{
    QString required1 = "<font color='"+COLOR_REQUIRED+"'>";
    QString required2 = "</font>";
    QString html = "";
    // Überschrift
    html += "<h2 class='pb'>"+(anlass != "" ? anlass : "Arbeitseinsatz")+" am " + datum.toString("dddd, dd.MM.yyyy")+"</h2>";
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

QString AActivity::getHtmlForTableView()
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
