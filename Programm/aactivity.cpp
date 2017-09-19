#include "aactivity.h"
#include "person.h"

#include <QMap>
#include <QDebug>
#include <QJsonArray>

Category AActivity::getCategoryFromString(QString s)
{
    s = s.toUpper();
    if (s=="TF") return Tf;
    if (s=="TB") return Tb;
    if (s=="ZF") return Zf;
    if (s=="SERVICE") return Service;
    if (s=="ZUG BEGLEITER" || s=="ZUB") return Zub;
    if (s=="BEGLEITER" || s=="BEGL.O.B.A.") return Begleiter;
    if (s=="BÜRO" || s=="BUERO") return Buero;
    if (s=="WERKSTATT") return Werkstatt;
    if (s=="VORBEREITEN" || s=="ZUG VORBEREITEN" || s=="ZUGVORBEREITEN") return ZugVorbereiten;
    return Sonstiges;
}

QString AActivity::getStringFromCategory(Category c)
{
    switch (c) {
    case Tf: return "Tf";
    case Tb: return "Tb";
    case Zf: return "Zf";
    case Service: return "Service";
    case Zub: return "Zub";
    case Begleiter: return "Begl.o.b.A.";
    case Buero: return "Büro";
    case Werkstatt: return "Werkstatt";
    case ZugVorbereiten: return "Zug Vorbereiten";
    default: return "Sonstiges";
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
    personen = new QMap<Person *, Infos*>();
    personalBenoetigt = true;
    personal = p;
}

AActivity::AActivity(QJsonObject o, ManagerPersonal *p)
{
    personal = p;
    personen = new QMap<Person*, Infos*>();

    datum = QDate::fromString(o.value("datum").toString(), "yyyy-MM-dd");
    ort = o.value("ort").toString();
    zeitAnfang = QTime::fromString(o.value("zeitAnfang").toString(), "hh:mm");
    zeitEnde = QTime::fromString(o.value("zeitEnde").toString(), "hh:mm");
    anlass = o.value("anlass").toString();
    bemerkungen = o.value("bemerkungen").toString();
    QJsonArray array = o.value("personen").toArray();
    for(int i = 0; i < array.size(); i++) {
        QJsonObject aO = array.at(i).toObject();
        QString name = aO.value("name").toString();
        Category kat = (Category) aO.value("kat").toInt();

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
        info->aufgabe = aO.value("aufgabe").toString();
        info->bemerkung = aO.value("bemerkung").toString();

        if (p != nullptr) {
            p->addActivity(this, kat);
            personen->insert(p, info);
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
    for(Person *p: personen->keys()) {
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
    for(Person *p: personen->keys()) {
        QJsonObject persJson;
        persJson.insert("name", p->getName());
        persJson.insert("beginn", personen->value(p)->beginn.toString("hh:mm"));
        persJson.insert("ende", personen->value(p)->ende.toString("hh:mm"));
        persJson.insert("kat", (int) personen->value(p)->kategorie);
        persJson.insert("aufgabe", personen->value(p)->aufgabe);
        persJson.insert("bemerkung", personen->value(p)->bemerkung);
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

QMap<Person *, AActivity::Infos *> *AActivity::getPersonen() const
{
    return personen;
}

bool AActivity::removePerson(Person *p)
{
    personen->remove(p);
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
    Person *gefunden;
    for(Person *pers: personen->keys()) {
        if (pers->getName() == p) {
            gefunden = pers;
            break;
        }
    }
    if (gefunden != nullptr) {
        personen->remove(gefunden);
        emitter();
        return true;
    }
    return false;
}

Misstake AActivity::addPerson(Person *p, QString bemerkung, QTime start, QTime ende, QString aufgabe)
{
   /*
    * angabe über aufgabe prüfen
    * p. ob person geeignet
    * person hinzufügen
    */
    Category kat = AActivity::getCategoryFromString(aufgabe);
    /**
      * TODO: Hier muss noch ordentlich Zeit reininvestiert werden.
      * Denn die bisherige Vorgehensweise ist überhlolt.
      * Die ganze verabreitung muss zentral hier in der MEthode bzw. Klasse stattfidnen
      * denn dann wird es im COde übersichtliche run ddas MVC-Prinzip wird besser umgesetzt
      *
      */
    switch (kat) {
    case Tf:
        if (! p->getAusbildungTf()) {
            return Misstake::FalscheQualifikation;
        }
        break;
    case Tb:
        // Ein Tb muss auch Lokführer sein, diese muss noch abgeklärt werden: Issue #9
        if (! p->getAusbildungTf()) {
            return Misstake::FalscheQualifikation;
        }
        break;
    case Zf:
        if (! p->getAusbildungZf()) {
            return Misstake::FalscheQualifikation;
        }
        break;
    case Zub:
        if (! p->getAusbildungRangierer()) {
            kat = Begleiter;
        }
        break;
    default:
        break;
    }

    // jetzt ist alles richtig und die person kann registiert werden.
    p->addActivity(this, kat);

    Infos *info = new Infos();
    info->beginn = start;
    info->ende = ende;
    info->kategorie = kat;
    info->aufgabe = aufgabe;
    info->bemerkung = bemerkung;

    personen->insert(p, info);

    emitter();
    return Misstake::OK;
}

Misstake AActivity::addPerson(QString p, QString bemerkung, QTime start, QTime ende, QString aufgabe)
{
    Person *pers = personal->getPerson(p);
    if (pers != nullptr) {
        return addPerson(pers, bemerkung, start, ende, aufgabe);
    }
    return Misstake::PersonNichtGefunden;
}

void AActivity::updatePersonBemerkung(Person *p, QString bemerkung)
{
    personen->value(p)->bemerkung = bemerkung;
}

void AActivity::setPersonen(QMap<Person *, Infos *> *value)
{
    personen = value;
    emitter();
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
        if (aufgabe) {
            if (liste->value(p)->aufgabe != "" && liste->value(p)->aufgabe != "Sonstiges") {
                if (! strichPunkt) {
                    a += "; ";
                    strichPunkt = true;
                } else {
                    a += " ";
                }
                a += liste->value(p)->aufgabe;
            } else if(liste->value(p)->kategorie != Category::Sonstiges) {
                if (! strichPunkt) {
                    a += "; ";
                    strichPunkt = true;
                } else {
                    a += " ";
                }
                a += AActivity::getStringFromCategory(liste->value(p)->kategorie);
            }
        }
        if (p != liste->keys().last()) {
            a += seperator;
        }
    }
    return a;
}

QComboBox *AActivity::generateNewCategoryComboBox()
{
    QComboBox *box = new QComboBox();
    box->insertItems(0, QStringList({"Tf","Tb","Zf","Service","Zug Begleiter","Büro","Werkstatt","Vorbereiten","Sonstiges"}));
    box->setCurrentIndex(8);
    return box;
}

QTimeEdit *AActivity::generateNewTimeEdit()
{
    QTimeEdit *edit = new QTimeEdit();
    edit->setDisplayFormat("hh:mm");
    return edit;
}
