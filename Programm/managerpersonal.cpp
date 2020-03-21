#include "managerpersonal.h"
#include "person.h"

#include <QJsonArray>

QHash<Category, int> ManagerPersonal::minimumHoursDefault {{Category::Tf, 100*60}, {Gesamt, 10*60}};


ManagerPersonal::ManagerPersonal()
{
    personen = QSet<Person*>();
    personenSorted = QMap<QString, Person*>();
    idToPerson = QMap<QString, Person*>();
    minimumHours = QMap<Category, int>();
    foreach (Category cat, minimumHoursDefault.keys()) {
       minimumHours.insert(cat, minimumHoursDefault.value(cat));
    }
}

ManagerPersonal::~ManagerPersonal()
{

}

QJsonObject ManagerPersonal::toJson()
{
    // Personen
    QJsonArray array;
    for(Person *p: personen.values()) {
        array.append(p->toJson());
    }
    // Mindesstunden
    QJsonArray keys;
    QJsonArray values;
    foreach (Category cat, minimumHours.keys()) {
        if (minimumHours.value(cat) > 0) {
            keys.append(int(cat));
            values.append(minimumHours.value(cat));
        }
    }

    QJsonObject o;
    o.insert("personen", array);
    o.insert("minimumKeys", keys);
    o.insert("minimumValues", values);
    return o;
}

QJsonObject ManagerPersonal::personalToJson()
{
    // Personen
    QJsonArray array;
    for(Person *p: personen.values()) {
        array.append(p->personalToJson());
    }
    // Mindesstunden
    QJsonArray keys;
    QJsonArray values;
    foreach (Category cat, minimumHours.keys()) {
        if (minimumHours.value(cat) > 0) {
            keys.append(int(cat));
            values.append(minimumHours.value(cat));
        }
    }

    QJsonObject o;
    o.insert("personen", array);
    o.insert("minimumKeys", keys);
    o.insert("minimumValues", values);
    return o;
}

void ManagerPersonal::fromJson(QJsonObject o)
{
    QJsonArray a = o.value("personen").toArray();
    for(int i = 0; i < a.size(); i++) {
        Person *neu = new Person(a.at(i).toObject(), this);
        personen.insert(neu);
        personenSorted.insert(neu->getName(), neu);
        idToPerson.insert(neu->getId(), neu);
        connect(neu, SIGNAL(nameChanged(Person*,QString)), this, SLOT(personChangedName(Person*,QString)));
    }
    if (o.contains("minimumKeys") && o.contains("minimumValues")) {
        QJsonArray keys = o.value("minimumKeys").toArray();
        QJsonArray values = o.value("minimumValues").toArray();
        minimumHours.clear();
        for (int i = 0; i < keys.size() && i < values.size() ; i++) {
            minimumHours.insert(Category(keys.at(i).toInt()), values.at(i).toInt());
        }
    } else {
        minimumHours.insert(Gesamt, int(o.value("minimumTotal").toDouble(0)*60));
        if (o.contains("minimumHours")) {
            minimumHours.clear();
            QJsonObject o2 = o.value("minimumHours").toObject();
            foreach (QString cat, o2.keys()) {
                Category catt = AActivity::getCategoryFromString(cat);
                minimumHours.insert(catt, int(o2.value(cat).toDouble(0)*60));
            }
        }
    }
}

Person *ManagerPersonal::getPersonFromID(QString id)
{
    return idToPerson.value(id);
}

Person *ManagerPersonal::getPerson(QString name)
{
    name = getGoodName(name);
    if (personExists(name)) {
        return personenSorted.value(name);
    }
    return nullptr;
}

bool ManagerPersonal::personExists(QString name)
{
    name = getGoodName(name);
    return personenSorted.contains(name);
}

bool ManagerPersonal::personExists(QString vorname, QString nachname)
{
    QString nameKomplett;
    if (vorname != "") nameKomplett = vorname + " " + nachname;
    else nameKomplett = nachname;
    return personenSorted.contains(nameKomplett);
}

Person *ManagerPersonal::newPerson()
{
    QString name = tr("Unbekannt Unbenannt");
    if (personExists(name)) return nullptr;
    Person *neu = new Person(name, this);
    personen.insert(neu);
    personenSorted.insert(name, neu);
    idToPerson.insert(neu->getId(), neu);
    connect(neu, SIGNAL(nameChanged(Person*,QString)), this, SLOT(personChangedName(Person*,QString)));
    return neu;
}

bool ManagerPersonal::removePerson(QString name)
{
    name = getGoodName(name);
    if (personExists(name)) {
        return removePerson(personenSorted.value(name));
    }
    return false;
}

bool ManagerPersonal::removePerson(Person *p)
{
    if (personen.contains(p)) {
        personen.remove(p);
        personenSorted.remove(p->getName());
        idToPerson.remove(p->getId());
        return true;
    }
    return false;
}

int ManagerPersonal::pruefeStunden(Person *p)
{
    bool ok = checkHours(p, Tf) && checkHours(p, Tb) &&
            checkHours(p, Zf) && checkHours(p, Zub) &&
            checkHours(p, Service) && checkHours(p, ZugVorbereiten) &&
            checkHours(p, Werkstatt) && checkHours(p, Buero) &&
            checkHours(p, Ausbildung) && checkHours(p, Sonstiges) &&
            checkHours(p, Gesamt);
    if (p->getAktiv() && (! ok))
        return 0;
    else if ((!p->getAktiv()) && ok)
        return -1;
    else
        return 1;
}

bool ManagerPersonal::checkHours(Person *p, Category kat)
{
    switch (kat) {
    case Tf:
        return !(p->getAusbildungTf() && p->get(Tf) < getMinimumHours(Category::Tf));
    case Tb:
        return true;
    case Zf:
        return !(p->getAusbildungZf() && p->get(Zf) < getMinimumHours(Category::Zf));
    case Service:
        return !(p->get(Service) < getMinimumHours(Service));
    case Zub:
        return !(p->get(Zub) < getMinimumHours(Zub));
    case Buero:
        return !(p->get(Buero) < getMinimumHours(Buero));
    case Werkstatt:
        return !(p->get(Werkstatt) < getMinimumHours(Werkstatt));
    case ZugVorbereiten:
        return !(p->get(ZugVorbereiten) < getMinimumHours(ZugVorbereiten));
    case Ausbildung:
        return !((p->getAusbildungRangierer() || p->getAusbildungTf() || p->getAusbildungZf()) && (p->get(Ausbildung) < getMinimumHours(Category::Ausbildung)));
    case Sonstiges:
        return !(p->get(Sonstiges) < getMinimumHours(Sonstiges));
    case Gesamt:
        return !(p->get(Gesamt) < getMinimumHours(Gesamt));
    default:
        return true;
    }
}

void ManagerPersonal::setMinimumHours(Category cat, int amount)
{
    minimumHours.insert(cat, amount);
}

int ManagerPersonal::getMinimumHours(Category cat)
{
    return minimumHours.value(cat, 0);
}

int ManagerPersonal::getMinimumHoursDefault(Category cat)
{
    return minimumHoursDefault.value(cat, 0);
}

void ManagerPersonal::personChangedName(Person *p, QString alt)
{
    personenSorted.remove(alt);
    personenSorted.insert(p->getName(), p);
}

void ManagerPersonal::reloadSettings()
{
    // Einstellungen neu laden, betrifft besonders die Zeiten, ab wann man nicht genug stunden hat
/*    mindestStunden = 10;
    mindestStundenTf = 0;
    mindestStundenZf = 0;*/
}

QListIterator<Person *> ManagerPersonal::getPersonen() const
{
    QListIterator<Person*> i(personenSorted.values());
    return i;
}

void ManagerPersonal::berechne()
{
    timeTf = 0;
    timeZf = 0;
    timeZub = 0;
    timeService = 0;
    timeBuero = 0;
    timeWerkstatt = 0;
    timeVorbereiten = 0;
    timeAusbildung = 0;
    timeSonstiges = 0;
    timeSum = 0;
    sumKilometer = 0;
    foreach (Person *p, personenSorted.values()) {
        p->berechne();
        timeTf += p->get(Tf);
        timeZf += p->get(Zf);
        timeZub += p->get(Zub);
        timeService += p->get(Service);
        timeBuero += p->get(Buero);
        timeWerkstatt += p->get(Werkstatt);
        timeVorbereiten += p->get(ZugVorbereiten);
        timeAusbildung += p->get(Ausbildung);
        timeSonstiges += p->get(Sonstiges);
        timeSum += p->get(Gesamt);
        sumKilometer += p->get(Kilometer);
    }
}

double ManagerPersonal::getTime(Category kat)
{
    switch (kat) {
    case Tf:
    case Tb:
        return timeTf;
    case Zf:
        return timeZf;
    case Zub:
    case Begleiter:
        return timeZub;
    case Service:
        return timeService;
    case ZugVorbereiten:
        return timeVorbereiten;
    case Werkstatt:
        return timeWerkstatt;
    case Buero:
        return timeBuero;
    case Ausbildung:
        return timeAusbildung;
    case Sonstiges:
        return timeSonstiges;
    case Gesamt:
        return timeSum;
    case Kilometer:
        return sumKilometer;
    default:
        return 0;
    }
}

QString ManagerPersonal::getGoodName(QString name)
{
    if (name.contains(QRegExp("\\s*,\\s*"))) {
        QStringList liste = name.split(QRegExp("\\s*,\\s*"));
        name = liste.at(1) + " " + liste.at(0);
    }
    return name;
}

int ManagerPersonal::getNextNummer()
{
    int max = 0;
    foreach (Person *p, personen.values()) {
        if (p->getNummer() > max) max = p->getNummer();
    }
    return max+1;
}

bool ManagerPersonal::checkNummer(int neu)
{
    QSet<Person*>::iterator i;
    for(i= personen.begin(); i != personen.end(); ++i) {
        if ((*i)->getNummer() == neu) return false;
    }
    return true;
}
