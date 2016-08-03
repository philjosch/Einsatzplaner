#ifndef MANAGERPERSONAL_H
#define MANAGERPERSONAL_H

class Person;

//#include "person.h"
#include <QObject>
#include <QSet>

class ManagerPersonal
{
public:
    ManagerPersonal();

    enum Misstake { OK, PersonNichtGefunden, FalscheQualifikation};

    Person *getPerson(QString name);
    bool personExists(QString name); // Prüft, ob die Person im System registriert ist

    Person *registerPerson(QString name); // Fügt die Person in das System ein
    bool removePerson(QString name);
    bool removePerson(Person *p);

private:
    QSet<Person*> *personen;
    QHash<QString, Person*> *personenSorted;

    QString getGoodName(QString name); // Wandlet einen Namen in das Format Vorname Nachname um
};

#endif // MANAGERPERSONAL_H
