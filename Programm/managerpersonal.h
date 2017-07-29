#ifndef MANAGERPERSONAL_H
#define MANAGERPERSONAL_H

#include <QJsonObject>
#include <QObject>
#include <QSet>

#include "enums.h"

class ManagerPersonal;

#include "aactivity.h"

class Person;

class ManagerPersonal :  public QObject
{
    Q_OBJECT

public:
    ManagerPersonal();
    ~ManagerPersonal();

    QJsonObject toJson();
    QJsonObject personalToJson();
    void fromJson(QJsonObject o);


    Person *getPerson(QString name);
    bool personExists(QString name); // Prüft, ob die Person im System registriert ist
    bool personExists(QString vorname, QString nachname); // Prüft, ob die Person im System registriert ist

    Person *registerPerson(QString vorname, QString nachname); // Fügt die Person in das System ein
    Person *registerPerson(QString name); // Fügt die Person in das System ein
    bool removePerson(QString name);
    bool removePerson(Person *p);

    static bool pruefeStunden(Person *p);

    void setMinimumHours(Category cat, double amount);
    void setMinimumHours(double amount);

    double getMinimumHours(Category cat);
    double getMinimumHours();

    static double getMinimumHoursDefault(Category kat);
    static double getMinimumHoursDefault();

    static double mindestStunden;
    static double mindestStundenTf;
    static double mindestStundenZf;

    QSetIterator<Person *> getPersonen() const;

public slots:
    void personChangedName(Person *p, QString alt);
    void reloadSettings();

private:
    QSet<Person *> *personen;
    QHash<QString, Person*> *personenSorted;

    QString getGoodName(QString name); // Wandlet einen Namen in das Format Vorname Nachname um

    QHash<Category, double> *minimumHours;
    double minimumTotal;

    static QHash<Category, double> minimumHoursDefault;
    static double minimumTotalDefault;

};

#endif // MANAGERPERSONAL_H
