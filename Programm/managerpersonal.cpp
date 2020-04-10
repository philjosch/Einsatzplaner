#include "managerpersonal.h"
#include "person.h"

#include <QJsonArray>

QMap<Category, int> ManagerPersonal::minimumHoursDefault {{Category::Tf, 100*60}, {Gesamt, 10*60}};


ManagerPersonal::ManagerPersonal()
{
    personen = QSet<Person*>();
    personenSorted = QMap<QString, Person*>();
    idToPerson = QMap<QString, Person*>();
    minimumHours = QMap<Category, int>();
    foreach (Category cat, minimumHoursDefault.keys()) {
       minimumHours.insert(cat, minimumHoursDefault.value(cat));
    }
    time = QMap<Category,int>();
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
                Category catt = getCategoryFromLocalizedString(cat);
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
            checkHours(p, Ausbildung) && checkHours(p, Infrastruktur) &&
            checkHours(p, Sonstiges) && checkHours(p, Gesamt);
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
        return !(p->get(Tf) < getMinimumHours(Tf, p));
    case Tb:
        return true;
    case Zf:
        return !(p->get(Zf) < getMinimumHours(Zf, p));
    case Service:
        return !(p->get(Service) < getMinimumHours(Service, p));
    case Zub:
        return !(p->get(Zub) < getMinimumHours(Zub, p));
    case Buero:
        return !(p->get(Buero) < getMinimumHours(Buero, p));
    case Werkstatt:
        return !(p->get(Werkstatt) < getMinimumHours(Werkstatt, p));
    case ZugVorbereiten:
        return !(p->get(ZugVorbereiten) < getMinimumHours(ZugVorbereiten, p));
    case Ausbildung:
        return !(p->get(Ausbildung) < getMinimumHours(Ausbildung, p));
    case Infrastruktur:
        return !(p->get(Infrastruktur) < getMinimumHours(Infrastruktur, p));
    case Sonstiges:
        return !(p->get(Sonstiges) < getMinimumHours(Sonstiges, p));
    case Gesamt:
        return !(p->get(Gesamt) < getMinimumHours(Gesamt, p));
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

int ManagerPersonal::getMinimumHours(Category cat, Person *p)
{
    switch (cat) {
    case Tf:
        if (! p->getAusbildungTf()) return 0;
        return minimumHours.value(cat, 0);
    case Zf:
        if (! p->getAusbildungZf()) return 0;
        return minimumHours.value(cat, 0);
    case Ausbildung:
        if (! p->getAusbildungTf() && ! p->getAusbildungZf() && ! p->getAusbildungRangierer()) return 0;
        return minimumHours.value(cat, 0);
    default:
        return minimumHours.value(cat, 0);
    }
}

QString ManagerPersonal::getMinimumHoursString(Category cat)
{
    int value = getMinimumHours(cat);
    return QString("%1:%2 h").arg(int(value/60)).arg(value % 60, 2, 10,QLatin1Char('0'));
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

QList<Person *> ManagerPersonal::getPersonenSortiertNachName()
{
    QList<Person *> l;
    int i = 0;
    foreach (Person *p, personen) {
        l.append(p);
        i = l.size()-1;
        while(i > 0 && l.at(i-1)->getName() > l.at(i)->getName()) {
            l.swapItemsAt(i, i-1);
            i--;
        }
    }
    return l;
}

QList<Person *> ManagerPersonal::getPersonenSortiertNachNummer()
{
    QList<Person *> l;
    int i = 0;
    foreach (Person *p, personen) {
        l.append(p);
        i = l.size()-1;
        while(i > 0 && l.at(i-1)->getNummer() > l.at(i)->getNummer()) {
            l.swapItemsAt(i, i-1);
            i--;
        }
    }
    return l;
}

void ManagerPersonal::berechne()
{
    time.clear();
    foreach (Person *p, personenSorted.values()) {
        p->berechne();
        time.insert(Tf, time.value(Tf)+p->get(Tf));
        time.insert(Zf, time.value(Zf)+p->get(Zf));
        time.insert(Zub, time.value(Zub)+p->get(Zub));
        time.insert(Service, time.value(Service)+p->get(Service));
        time.insert(Buero, time.value(Buero)+p->get(Buero));
        time.insert(Werkstatt, time.value(Werkstatt)+p->get(Werkstatt));
        time.insert(ZugVorbereiten, time.value(ZugVorbereiten)+p->get(ZugVorbereiten));
        time.insert(Ausbildung, time.value(Ausbildung)+p->get(Ausbildung));
        time.insert(Infrastruktur, time.value(Infrastruktur)+p->get(Infrastruktur));
        time.insert(Sonstiges, time.value(Sonstiges)+p->get(Sonstiges));
        time.insert(Gesamt, time.value(Gesamt)+p->get(Gesamt));
        time.insert(Kilometer, time.value(Kilometer)+p->get(Kilometer));
    }
    time.insert(Anzahl, 0);
}

int ManagerPersonal::getTime(Category kat)
{
    return time.value(kat, 0);
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
