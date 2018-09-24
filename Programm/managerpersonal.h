#ifndef MANAGERPERSONAL_H
#define MANAGERPERSONAL_H

#include <QJsonObject>
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

    bool pruefeStunden(Person *p); // Gibt true zurueck, genau dann wenn die Person die notwenidgen stunden erbracht hat!
    bool checkHours(Person *p, Category kat); // Gibt true zurueck, genau dann wenn die Person die notwenidgen stunden erbracht hat!
    bool checkHours(Person *p); // Gibt true zurueck, genau dann wenn die Person die notwenidgen stunden erbracht hat!

    void setMinimumHours(Category cat, double amount);
    void setMinimumHours(double amount);

    double getMinimumHours(Category cat);
    double getMinimumHours();

    static double getMinimumHoursDefault(Category kat);
    static double getMinimumHoursDefault();

    QSetIterator<Person *> getPersonen() const;

    void berechne();

    double getTime(Category kat);
    double getTimeTf() const;
    double getTimeZf() const;
    double getTimeZub() const;
    double getTimeService() const;
    double getTimeBuero() const;
    double getTimeWerkstatt() const;
    double getTimeVorbereiten() const;
    double getTimeAusbildung() const;
    double getTimeSonstiges() const;
    double getTimeSum() const;
    double getSumKilometer() const;

    static QString getGoodName(QString name); // Wandlet einen Namen in das Format Vorname Nachname um

public slots:
    void personChangedName(Person *p, QString alt);
    void reloadSettings();

private:
    QSet<Person *> personen;
    QHash<QString, Person*> personenSorted;

    QHash<Category, double> minimumHours;
    double minimumTotal;

    static QHash<Category, double> minimumHoursDefault;
    static double minimumTotalDefault;

    double timeTf;
    double timeZf;
    double timeZub;
    double timeService;
    double timeBuero;
    double timeWerkstatt;
    double timeVorbereiten;
    double timeAusbildung;
    double timeSonstiges;
    double timeSum;
    double sumKilometer;

};

#endif // MANAGERPERSONAL_H
