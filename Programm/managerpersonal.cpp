#include "managerpersonal.h"
#include "person.h"

#include <QJsonArray>
#include <QList>
#include <QObject>
#include <QSetIterator>

QHash<Category, double> ManagerPersonal::minimumHoursDefault {{Category::Tf, 100}};
double ManagerPersonal::minimumTotalDefault = 10.0;


ManagerPersonal::ManagerPersonal()
{
    personen = new QSet<Person*>();
    personenSorted = new QHash<QString, Person*>();
    minimumHours = new QHash<Category, double>();
    foreach (Category cat, minimumHoursDefault.keys()) {
       minimumHours->insert(cat, minimumHoursDefault.value(cat));
    }
    minimumTotal = minimumTotalDefault;
}

ManagerPersonal::~ManagerPersonal()
{

}

QJsonObject ManagerPersonal::toJson()
{
    // TODO inlcude minimum Hours
    QJsonArray array;
    for(Person *p: personen->values()) {
        array.append(p->toJson());
    }
    QJsonObject o2;
    foreach (Category cat, minimumHours->keys()) {
       o2.insert(AActivity::getStringFromCategory(cat), minimumHours->value(cat));
    }

    QJsonObject o;
    o.insert("personen", array);
    o.insert("minimumHours", o2);
    o.insert("minimumTotal", minimumTotal);
    return o;
}

QJsonObject ManagerPersonal::personalToJson()
{
    // TODO: include MinimumHours
    QJsonArray array;
    for(Person *p: personen->values()) {
        array.append(p->personalToJson());
    }
    QJsonObject o2;
    foreach (Category cat, minimumHours->keys()) {
       o2.insert(AActivity::getStringFromCategory(cat), minimumHours->value(cat));
    }

    QJsonObject o;
    o.insert("personen", array);
    o.insert("minimumHours", o2);
    o.insert("minimumTotal", minimumTotal);
    return o;
}

void ManagerPersonal::fromJson(QJsonObject o)
{
    // TODO: minimumHours
    QJsonArray a = o.value("personen").toArray();
    for(int i = 0; i < a.size(); i++) {
        Person *neu = Person::fromJson(a.at(i).toObject(), this);
        personen->insert(neu);
        personenSorted->insert(neu->getName(), neu);
        connect(neu, SIGNAL(nameChanged(Person*,QString)), this, SLOT(personChangedName(Person*,QString)));
    }
    if (o.contains("minimumTotal")) {
        minimumTotal = o.value("minimumTotal").toDouble(minimumTotalDefault);
    }
    if (o.contains("minimumHours")) {
        minimumHours = new QHash<Category, double>();
        QJsonObject o2 = o.value("minimumHours").toObject();
        foreach (QString cat, o2.keys()) {
            Category catt = AActivity::getCategoryFromString(cat);
            minimumHours->insert(catt, o2.value(cat).toDouble(0));
        }
    }
}

Person *ManagerPersonal::getPerson(QString name)
{
    name = getGoodName(name);
    if (personExists(name)) {
        return personenSorted->value(name);
    }
    return nullptr;
}

bool ManagerPersonal::personExists(QString name)
{
    name = getGoodName(name);
    return personenSorted->contains(name);
}

bool ManagerPersonal::personExists(QString vorname, QString nachname)
{
    QString nameKomplett;
    if (vorname != "") nameKomplett = vorname + " " + nachname;
    else nameKomplett = nachname;
    return personenSorted->contains(nameKomplett);
}

Person *ManagerPersonal::registerPerson(QString vorname, QString  nachname)
{
    QString nameKomplett;
    if (vorname != "") nameKomplett = vorname + " " + nachname;
    else nameKomplett = nachname;
    if (! personExists(nameKomplett)) {
        Person *neu = new Person(vorname, nachname, this);
        personen->insert(neu);
        personenSorted->insert(nameKomplett, neu);
        connect(neu, SIGNAL(nameChanged(Person*,QString)), this, SLOT(personChangedName(Person*,QString)));
        return neu;
    }
    return nullptr;
}

Person *ManagerPersonal::registerPerson(QString name)
{
    if (! personExists(name)) {
        Person *neu = new Person(name, this);
        personen->insert(neu);
        personenSorted->insert(name, neu);
        connect(neu, SIGNAL(nameChanged(Person*,QString)), this, SLOT(personChangedName(Person*,QString)));
        return neu;
    }
    return nullptr;
}

bool ManagerPersonal::removePerson(QString name)
{
    name = getGoodName(name);
    if (personExists(name)) {
        return removePerson(personenSorted->value(name));
    }
    return false;
}

bool ManagerPersonal::removePerson(Person *p)
{
    if (personen->contains(p)) {
        personen->remove(p);
        personenSorted->remove(p->getName());
        return true;
    }
    return false;
}

bool ManagerPersonal::pruefeStunden(Person *p)
{
    if (p->getAusbildungTf())
        if (p->getTimeTf() < getMinimumHours(Category::Tf))
            return false;
    if (p->getAusbildungZf())
        if (p->getTimeZf() < getMinimumHours(Category::Zf))
            return false;
    if (p->getTimeService() < getMinimumHours(Category::Service))
        return false;
    if (p->getTimeZub() < getMinimumHours(Category::Zub))
        return false;
    if (p->getTimeBuero() < getMinimumHours(Category::Buero))
        return false;
    if (p->getTimeWerkstatt() < getMinimumHours(Category::Werkstatt))
        return false;
    if (p->getTimeVorbereiten() < getMinimumHours(Category::ZugVorbereiten))
        return false;
    if (p->getTimeSonstiges() < getMinimumHours(Category::Sonstiges))
        return false;
    if (p->getTimeSum() < getMinimumHours())
        return false;
    return true;
}

void ManagerPersonal::setMinimumHours(Category cat, double amount)
{
    minimumHours->insert(cat, amount);
}

void ManagerPersonal::setMinimumHours(double amount)
{
    minimumTotal = amount;
}

double ManagerPersonal::getMinimumHours(Category cat)
{
    return minimumHours->value(cat, 0);
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
    personenSorted->remove(alt);
    personenSorted->insert(p->getName(), p);
}

void ManagerPersonal::reloadSettings()
{
    // Einstellungen neu laden, betrifft besonders die Zeiten, ab wann man nicht genug stunden hat
/*    mindestStunden = 10;
    mindestStundenTf = 0;
    mindestStundenZf = 0;*/
}

QSetIterator<Person *> ManagerPersonal::getPersonen() const
{
    QSetIterator<Person*> i(*personen);
    return i;
}

double ManagerPersonal::getTimeTf()
{
    double sum = 0;
    foreach (Person *p, personenSorted->values()) sum += p->getTimeTf();
    return sum;
}

double ManagerPersonal::getTimeZf()
{
    double sum = 0;
    foreach (Person *p, personenSorted->values()) sum += p->getTimeZf();
    return sum;
}

double ManagerPersonal::getTimeZub()
{
    double sum = 0;
    foreach (Person *p, personenSorted->values()) sum += p->getTimeZub();
    return sum;
}

double ManagerPersonal::getTimeService()
{
    double sum = 0;
    foreach (Person *p, personenSorted->values()) sum += p->getTimeService();
    return sum;
}

double ManagerPersonal::getTimeBuero()
{
    double sum = 0;
    foreach (Person *p, personenSorted->values()) sum += p->getTimeBuero();
    return sum;
}

double ManagerPersonal::getTimeWerkstatt()
{
    double sum = 0;
    foreach (Person *p, personenSorted->values()) sum += p->getTimeWerkstatt();
    return sum;
}

double ManagerPersonal::getTimeVorbereiten()
{
    double sum = 0;
    foreach (Person *p, personenSorted->values()) sum += p->getTimeVorbereiten();
    return sum;
}

double ManagerPersonal::getTimeSonstiges()
{
    double sum = 0;
    foreach (Person *p, personenSorted->values()) sum += p->getTimeSonstiges();
    return sum;
}

double ManagerPersonal::getTimeSum()
{
    double sum = 0;
    foreach (Person *p, personenSorted->values()) sum += p->getTimeSum();
    return sum;
}

double ManagerPersonal::getSumKilometer()
{
    double sum = 0;
    foreach (Person *p, personenSorted->values()) sum += p->getSumKilometer();
    return sum;
}

QString ManagerPersonal::getGoodName(QString name)
{
    if (name.contains(QRegExp("\\s*,\\s*"))) {
        QStringList liste = name.split(QRegExp("\\s*,\\s*"));
        name = liste.at(1) + " " + liste.at(0);
    }
    return name;
}
