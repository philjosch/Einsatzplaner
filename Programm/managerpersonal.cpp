#include "managerpersonal.h"
#include "person.h"
#include <QList>
#include <QObject>
#include <QSetIterator>

double ManagerPersonal::mindestStunden = 10.f;
double ManagerPersonal::mindestStundenTf = 100.f;
double ManagerPersonal::mindestStundenZf = 0.f;

ManagerPersonal::ManagerPersonal()
{
    personen = new QSet<Person*>();
    personenSorted = new QHash<QString, Person*>();
}

ManagerPersonal::~ManagerPersonal()
{

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

Person *ManagerPersonal::registerPerson(QString name)
{
    name = getGoodName(name);
    if (! personExists(name)) {
        Person *neu = new Person(name);
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
    if ((p->getTimeSum() < mindestStunden) || (p->getTimeTf() < mindestStundenTf && p->getAusbildungTf()) || (p->getTimeZf() < mindestStundenZf && p->getAusbildungZf()) ) {
        return false;
    }
    return true;
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

QString ManagerPersonal::getGoodName(QString name)
{
    if (name.contains(", ")) {
        QStringList liste = name.split(", ");
        name = liste.at(1)+liste.at(0);
    }
    return name;
}
