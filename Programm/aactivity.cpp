#include "aactivity.h"
#include "person.h"
#include "fahrtag.h"
#include "activity.h"

#include <QJsonArray>

QStringList AActivity::EXTERNAL_LIST = QStringList() << "Extern" << "Führerstand" << "FS" << "Schnupperkurs" << "ELF" << "Ehrenlokführer" << "ELF-Kurs";
QStringList AActivity::QUALIFICATION_LIST = QStringList() << "Azubi" << "Ausbildung" << "Tf-Ausbildung" << "Zf-Ausbildung" << "Tf-Unterricht" << "Zf-Unterricht" << "Weiterbildung";
QString AActivity::COLOR_REQUIRED = "#ff3333";

Category AActivity::getCategoryFromString(QString s)
{
    s = s.toUpper();
    if (s=="TF") return Tf;
    if (s=="TB") return Tb;
    if (s=="ZF") return Zf;
    if (s=="SERVICE") return Service;
    if (s=="ZUGBEGLEITER" || s=="ZUB") return Zub;
    if (s=="BEGLEITER" || s=="BEGL.O.B.A.") return Begleiter;
    if (s=="BÜRO" || s=="BUERO") return Buero;
    if (s=="WERKSTATT") return Werkstatt;
    if (s=="VORBEREITEN" || s=="ZUG VORBEREITEN" || s=="ZUGVORBEREITEN") return ZugVorbereiten;
    if (s=="AUSBILDUNG") return Ausbildung;
    return Sonstiges;
}

QString AActivity::getStringFromCategory(Category c)
{
    switch (c) {
    case Tf: return QObject::tr("Tf");
    case Tb: return QObject::tr("Tb");
    case Zf: return QObject::tr("Zf");
    case Service: return QObject::tr("Service");
    case Zub: return QObject::tr("Zugbegleiter");
    case Begleiter: return QObject::tr("Begl.o.b.A.");
    case Buero: return QObject::tr("Büro");
    case Werkstatt: return QObject::tr("Werkstatt");
    case ZugVorbereiten: return QObject::tr("Zug Vorbereiten");
    case Ausbildung: return QObject::tr("Ausbildung");
    default: return QObject::tr("Sonstiges");
    }
}

AActivity::AActivity(QDate date, ManagerPersonal *p)
{
    datum = date;
    ort = "";
    zeitAnfang = QTime(10, 0);
    zeitEnde = QTime(16, 0);
    anlass = "";
    bemerkungen = "";
    personen = QMap<Person *, Infos*>();
    personalBenoetigt = true;
    personal = p;
}

AActivity::AActivity(QJsonObject o, ManagerPersonal *p)
{
    personal = p;
    personen = QMap<Person*, Infos*>();

    datum = QDate::fromString(o.value("datum").toString(), "yyyy-MM-dd");
    ort = o.value("ort").toString();
    zeitAnfang = QTime::fromString(o.value("zeitAnfang").toString(), "hh:mm");
    zeitEnde = QTime::fromString(o.value("zeitEnde").toString(), "hh:mm");
    anlass = o.value("anlass").toString();
    bemerkungen = o.value("bemerkungen").toString().replace("<br/>", "\n");
    QJsonArray array = o.value("personen").toArray();
    for(int i = 0; i < array.size(); i++) {
        QJsonObject aO = array.at(i).toObject();
        QString name = aO.value("name").toString();
        Category kat = static_cast<Category>(aO.value("kat").toInt(100));

        Person *p;
        if (personal->personExists(name)) {
            p = personal->getPerson(name);
        } else {
            p = new Person(name, nullptr);
            p->setAusbildungTf(true);
            p->setAusbildungZf(true);
            p->setAusbildungRangierer(true);
        }
        Infos *info = new Infos();
        info->beginn = QTime::fromString(aO.value("beginn").toString(), "hh:mm");
        info->ende = QTime::fromString(aO.value("ende").toString(), "hh:mm");
        info->kategorie = kat;
        info->bemerkung = aO.value("bemerkung").toString();

        if (p != nullptr) {
            p->addActivity(this, kat);
            personen.insert(p, info);
        }

    }
    personalBenoetigt = o.value("personalBenoetigt").toBool();
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
    data.insert("anlass", anlass);
    data.insert("bemerkungen", bemerkungen);
    QJsonArray personenJSON;
    for(Person *p: personen.keys()) {
        QJsonObject persJson;
        persJson.insert("name", p->getName());
        persJson.insert("beginn", personen.value(p)->beginn.toString("hh:mm"));
        persJson.insert("ende", personen.value(p)->ende.toString("hh:mm"));
        persJson.insert("kat", personen.value(p)->kategorie);
        persJson.insert("bemerkung", personen.value(p)->bemerkung);
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
    datum = value;
    emitter();
}

QString AActivity::getOrt() const
{
    return ort;
}

void AActivity::setOrt(const QString &value)
{
    ort = value;
    emitter();
}

QTime AActivity::getZeitAnfang()
{
    return zeitAnfang;
}

void AActivity::setZeitAnfang(QTime value)
{
    zeitAnfang = value;
    emitter();
}

QTime AActivity::getZeitEnde()
{
    return zeitEnde;
}

void AActivity::setZeitEnde(QTime value)
{
    zeitEnde = value;
    emitter();
}

QString AActivity::getAnlass() const
{
    return anlass;
}

void AActivity::setAnlass(const QString &value)
{
    anlass = value;
    emitter();
}

QString AActivity::getBemerkungen() const
{
    return bemerkungen;
}

void AActivity::setBemerkungen(const QString &value)
{
    bemerkungen = value;
    emitter();
}

bool AActivity::getPersonalBenoetigt() const
{
    return personalBenoetigt;
}

void AActivity::setPersonalBenoetigt(bool value)
{
    personalBenoetigt = value;
    emitter();
}

QMap<Person *, AActivity::Infos *> AActivity::getPersonen()
{
    return personen;
}

bool AActivity::removePerson(Person *p)
{
    personen.remove(p);
    emitter();
    return p->removeActivity(this);
}

bool AActivity::removePerson(QString p)
{
    Person *pers = personal->getPerson(p);
    if (pers != nullptr) {
        emitter();
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
        emitter();
        return true;
    }
    return false;
}

Mistake AActivity::addPerson(Person *p, QString bemerkung, QTime start, QTime ende, Category kat)
{
   /*
    * angabe über aufgabe prüfen
    * p. ob person geeignet
    * person hinzufügen
    */

    if (kat == Zub && !hasQualification(p, kat, bemerkung)) kat = Begleiter;

    if (! hasQualification(p, kat, bemerkung)) return Mistake::FalscheQualifikation;

    // jetzt ist alles richtig und die person kann registiert werden.
    p->addActivity(this, kat);

    Infos *info = new Infos();
    info->beginn = start;
    info->ende = ende;
    info->kategorie = kat;
    info->bemerkung = bemerkung;

    personen.insert(p, info);

    emitter();
    return Mistake::OK;
}

Mistake AActivity::addPerson(QString p, QString bemerkung, QTime start, QTime ende, Category kat)
{
    Person *pers = personal->getPerson(p);
    if (pers != nullptr) {
        return addPerson(pers, bemerkung, start, ende, kat);
    }
    if (isExtern(bemerkung)) {
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

void AActivity::updatePersonBemerkung(Person *p, QString bemerkung)
{
    personen.value(p)->bemerkung = bemerkung;
}

bool AActivity::operator >(const AActivity &second) const
{
    return second.operator<(*this);
}

bool AActivity::operator <(const AActivity &second) const
{
    // Datum
    if (this->datum < second.datum)
        return true;
    else if (this->datum > second.datum)
        return false;
    // Beginn
    if (this->zeitAnfang < second.zeitAnfang)
        return true;
    else if (this->zeitAnfang > second.zeitAnfang)
        return false;
    // Ende
    if (this->zeitEnde < second.zeitEnde)
        return true;
    else if (this->zeitEnde > second.zeitEnde)
        return false;
    // Art und beliebig, bei gleicher Art
    if (const Fahrtag *f = dynamic_cast<const Fahrtag*>(this))
        return true;
    if (const Activity *a = dynamic_cast<const Activity*>(&second))
        return true;
    return false;
}

bool AActivity::operator ==(const AActivity &second) const
{
    // Datum
    if (this->datum != second.datum)
        return false;
    // Beginn
    if (this->zeitAnfang != second.zeitAnfang)
        return false;
    // Ende
    if (this->zeitEnde != second.zeitEnde)
        return false;
    // Art und beliebig, bei gleicher Art
    if (const Fahrtag *f = dynamic_cast<const Fahrtag*>(this)) {
        if (const Fahrtag *g = dynamic_cast<const Fahrtag*>(&second))
            return true;
        else
            return false;
    } else {
        if (const Activity *a = dynamic_cast<const Activity*>(&second))
            return true;
        return false;
    }
}

ManagerPersonal *AActivity::getPersonal() const
{
    return personal;
}

QString AActivity::listToString(QMap<Person *, AActivity::Infos *> *liste, QString seperator, bool aufgabe)
{
    QString a = "";
    for(Person *p: liste->keys()) {
        a+= p->getName();
        bool strichPunkt = false;
        if (liste->value(p)->bemerkung!= "") {
            a += "; "+liste->value(p)->bemerkung;
            strichPunkt = true;
        }
        if (aufgabe && (liste->value(p)->kategorie != Category::Sonstiges)) {
            if (strichPunkt) {
                a += ", ";
            } else {
                a += "; ";
                strichPunkt = true;
            }
            a += AActivity::getStringFromCategory(liste->value(p)->kategorie);
        }
        if (p != liste->keys().last()) {
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

QComboBox *AActivity::generateNewCategoryComboBox()
{
    QComboBox *box = new QComboBox();
    box->insertItems(0, QStringList({"Tf","Tb","Zf","Service","Zugbegleiter","Büro","Werkstatt","Zug Vorbereiten","Ausbildung","Sonstiges"}));
    box->setCurrentIndex(9);
    return box;
}

QTimeEdit *AActivity::generateNewTimeEdit()
{
    QTimeEdit *edit = new QTimeEdit();
    edit->setDisplayFormat("hh:mm");
    return edit;
}
