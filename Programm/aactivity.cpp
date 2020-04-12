#include "aactivity.h"
#include "person.h"
#include "fahrtag.h"
#include "activity.h"

#include <QJsonArray>
#include <QLinkedList>

QStringList AActivity::EXTERNAL_LIST = QStringList() << "Extern" << "Führerstand" << "FS" << "Schnupperkurs" << "ELF" << "Ehrenlokführer" << "ELF-Kurs";
QStringList AActivity::QUALIFICATION_LIST = QStringList() << "Azubi" << "Ausbildung" << "Tf-Ausbildung" << "Zf-Ausbildung" << "Tf-Unterricht" << "Zf-Unterricht" << "Weiterbildung";
QString AActivity::COLOR_REQUIRED = "#ff3333";


AActivity::AActivity(QDate date, ManagerPersonal *p)
{
    datum = date;
    ort = "";
    zeitAnfang = QTime(10, 0);
    zeitEnde = QTime(16, 0);
    anlass = "";
    bemerkungen = "";
    personen = QMap<Person *, Infos>();
    personalBenoetigt = true;
    personal = p;
}

AActivity::AActivity(QJsonObject o, ManagerPersonal *p)
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
    data.insert("isFahrtag", false);
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

    if (! hasQualification(p, kat, bemerkung)) return Mistake::FalscheQualifikation;

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

bool AActivity::hasQualification(Person *p, Category kat, QString bemerkung)
{
    switch (kat) {
    case Tf:
        if (p->getAusbildungTf()) return true;
        break;
    case Tb:
        if (p->getAusbildungZf()) return true;
        break;
    case Zf:
        if (p->getAusbildungZf()) return true;
        break;
    case Zub:
        if (p->getAusbildungRangierer()) return true;
        break;
    default:
        return true;
    }

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

const QString getFarbe(AActivity *a)
{
    if (Fahrtag *f = dynamic_cast<Fahrtag*>(a))
        return FARBE_FAHRTAGE.value(f->getArt());
    else
        return FARBE_FAHRTAGE.value(Arbeitseinsatz);
}
