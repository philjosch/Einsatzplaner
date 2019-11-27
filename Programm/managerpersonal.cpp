#include "managerpersonal.h"
#include "person.h"

#include <QJsonArray>

QHash<Category, double> ManagerPersonal::minimumHoursDefault {{Category::Tf, 100}};
double ManagerPersonal::minimumTotalDefault = 10.0;


ManagerPersonal::ManagerPersonal()
{
    personen = QSet<Person*>();
    personenSorted = QMap<QString, Person*>();
    minimumHours = QHash<Category, double>();
    foreach (Category cat, minimumHoursDefault.keys()) {
       minimumHours.insert(cat, minimumHoursDefault.value(cat));
    }
    minimumTotal = minimumTotalDefault;
}

ManagerPersonal::~ManagerPersonal()
{

}

QJsonObject ManagerPersonal::toJson()
{
    QJsonArray array;
    for(Person *p: personen.values()) {
        array.append(p->toJson());
    }
    QJsonObject o2;
    foreach (Category cat, minimumHours.keys()) {
       o2.insert(AActivity::getStringFromCategory(cat), minimumHours.value(cat));
    }

    QJsonObject o;
    o.insert("personen", array);
    o.insert("minimumHours", o2);
    o.insert("minimumTotal", minimumTotal);
    return o;
}

QJsonObject ManagerPersonal::personalToJson()
{
    QJsonArray array;
    for(Person *p: personen.values()) {
        array.append(p->personalToJson());
    }
    QJsonObject o2;
    foreach (Category cat, minimumHours.keys()) {
       o2.insert(AActivity::getStringFromCategory(cat), minimumHours.value(cat));
    }

    QJsonObject o;
    o.insert("personen", array);
    o.insert("minimumHours", o2);
    o.insert("minimumTotal", minimumTotal);
    return o;
}

void ManagerPersonal::fromJson(QJsonObject o)
{
    QJsonArray a = o.value("personen").toArray();
    for(int i = 0; i < a.size(); i++) {
        Person *neu = Person::fromJson(a.at(i).toObject(), this);
        personen.insert(neu);
        personenSorted.insert(neu->getName(), neu);
        connect(neu, SIGNAL(nameChanged(Person*,QString)), this, SLOT(personChangedName(Person*,QString)));
    }
    if (o.contains("minimumTotal")) {
        minimumTotal = o.value("minimumTotal").toDouble(minimumTotalDefault);
    }
    if (o.contains("minimumHours")) {
        minimumHours = QHash<Category, double>();
        QJsonObject o2 = o.value("minimumHours").toObject();
        foreach (QString cat, o2.keys()) {
            Category catt = AActivity::getCategoryFromString(cat);
            minimumHours.insert(catt, o2.value(cat).toDouble(0));
        }
    }
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

Person *ManagerPersonal::registerPerson(QString vorname, QString  nachname)
{
    QString nameKomplett;
    if (vorname != "") nameKomplett = vorname + " " + nachname;
    else nameKomplett = nachname;
    if (! personExists(nameKomplett)) {
        Person *neu = new Person(vorname, nachname, this);
        personen.insert(neu);
        personenSorted.insert(nameKomplett, neu);
        connect(neu, SIGNAL(nameChanged(Person*,QString)), this, SLOT(personChangedName(Person*,QString)));
        return neu;
    }
    return nullptr;
}

Person *ManagerPersonal::registerPerson(QString name)
{
    if (! personExists(name)) {
        Person *neu = new Person(name, this);
        personen.insert(neu);
        personenSorted.insert(name, neu);
        connect(neu, SIGNAL(nameChanged(Person*,QString)), this, SLOT(personChangedName(Person*,QString)));
        return neu;
    }
    return nullptr;
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
        return true;
    }
    return false;
}

bool ManagerPersonal::pruefeStunden(Person *p)
{
    return checkHours(p, Tf) && checkHours(p, Tb) &&
            checkHours(p, Zf) && checkHours(p, Zub) &&
            checkHours(p, Service) && checkHours(p, ZugVorbereiten) &&
            checkHours(p, Werkstatt) && checkHours(p, Buero) &&
            checkHours(p, Ausbildung) && checkHours(p, Sonstiges) &&
            checkHours(p);
}

bool ManagerPersonal::checkHours(Person *p, Category kat)
{
    switch (kat) {
    case Tf:
        return !(p->getAusbildungTf() && p->getTimeTf() < getMinimumHours(Category::Tf));
    case Tb:
        return true;
    case Zf:
        return !(p->getAusbildungZf() && p->getTimeZf() < getMinimumHours(Category::Zf));
    case Service:
        return !(p->getTimeService() < getMinimumHours(Category::Service));
    case Zub:
        return !(p->getTimeZub() < getMinimumHours(Category::Zub));
    case Buero:
        return !(p->getTimeBuero() < getMinimumHours(Category::Buero));
    case Werkstatt:
        return !(p->getTimeWerkstatt() < getMinimumHours(Category::Werkstatt));
    case ZugVorbereiten:
        return !(p->getTimeVorbereiten() < getMinimumHours(Category::ZugVorbereiten));
    case Ausbildung:
        return !((p->getAusbildungRangierer() || p->getAusbildungTf() || p->getAusbildungZf()) && (p->getTimeAusbildung() < getMinimumHours(Category::Ausbildung)));
    case Sonstiges:
        return !(p->getTimeSonstiges() < getMinimumHours(Category::Sonstiges));
    case Gesamt:
        return !(p->getTimeSum() < getMinimumHours());
    default:
        return true;
    }
}

bool ManagerPersonal::checkHours(Person *p)
{
    return checkHours(p, Category::Gesamt);
}

void ManagerPersonal::setMinimumHours(Category cat, double amount)
{
    minimumHours.insert(cat, amount);
}

void ManagerPersonal::setMinimumHours(double amount)
{
    minimumTotal = amount;
}

double ManagerPersonal::getMinimumHours(Category cat)
{
    return minimumHours.value(cat, 0);
}

double ManagerPersonal::getMinimumHours()
{
    return minimumTotal;
}

double ManagerPersonal::getMinimumHoursDefault(Category cat)
{
    return minimumHoursDefault.value(cat, 0);
}

double ManagerPersonal::getMinimumHoursDefault()
{
    return ManagerPersonal::minimumTotalDefault;
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
        timeTf += p->getTimeTf();
        timeZf += p->getTimeZf();
        timeZub += p->getTimeZub();
        timeService += p->getTimeService();
        timeBuero += p->getTimeBuero();
        timeWerkstatt += p->getTimeWerkstatt();
        timeVorbereiten += p->getTimeVorbereiten();
        timeAusbildung += p->getTimeAusbildung();
        timeSonstiges += p->getTimeSonstiges();
        timeSum += p->getTimeSum();
        sumKilometer += p->getSumKilometer();
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

double ManagerPersonal::getSumKilometer() const
{
    return sumKilometer;
}

double ManagerPersonal::getTimeSum() const
{
    return timeSum;
}

double ManagerPersonal::getTimeSonstiges() const
{
    return timeSonstiges;
}

double ManagerPersonal::getTimeVorbereiten() const
{
    return timeVorbereiten;
}

double ManagerPersonal::getTimeAusbildung() const
{
    return timeAusbildung;
}

double ManagerPersonal::getTimeWerkstatt() const
{
    return timeWerkstatt;
}

double ManagerPersonal::getTimeBuero() const
{
    return timeBuero;
}

double ManagerPersonal::getTimeService() const
{
    return timeService;
}

double ManagerPersonal::getTimeZub() const
{
    return timeZub;
}

double ManagerPersonal::getTimeZf() const
{
    return timeZf;
}

double ManagerPersonal::getTimeTf() const
{
    return timeTf;
}
