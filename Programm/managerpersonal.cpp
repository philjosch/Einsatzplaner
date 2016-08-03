#include "managerpersonal.h"
#include "person.h"
#include <QList>

ManagerPersonal::ManagerPersonal()
{
    personen = new QSet<Person*>();
    personenSorted = new QHash<QString, Person*>();
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

QString ManagerPersonal::getGoodName(QString name)
{
    if (name.contains(", ")) {
        QStringList liste = name.split(", ");
        name = liste.at(1)+liste.at(0);
    }
    return name;
}
