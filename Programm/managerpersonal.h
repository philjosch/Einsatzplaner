#ifndef MANAGERPERSONAL_H
#define MANAGERPERSONAL_H

#include <QJsonObject>
#include <QSet>

#include "basics.h"

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


    Person *getPersonFromID(QString id);
    Person *getPerson(QString name);
    bool personExists(QString name); // Prüft, ob die Person im System registriert ist
    bool personExists(QString vorname, QString nachname); // Prüft, ob die Person im System registriert ist

    Person *newPerson(); // Erstellt eine neue Person und gibt sie zurück
    bool removePerson(QString name);
    bool removePerson(Person *p);

    int pruefeStunden(Person *p); // Gibt true zurueck, genau dann wenn die Person fuer **alle** Kategorien die notwendigen Stunden erbracht hat!
    bool checkHours(Person *p, Category kat); // Gibt true zurueck, genau dann wenn die Person die notwendigen Stunden fuer die Kategorie erbracht hat!

    void setMinimumHours(Category cat, int amount);

    int getMinimumHours(Category cat);
    int getMinimumHours(Category cat, Person *p);
    QString getMinimumHoursString(Category cat);

    static int getMinimumHoursDefault(Category kat);

    QListIterator<Person *> getPersonen() const;
    QList<Person *> getPersonenSortiertNachName();
    QList<Person *> getPersonenSortiertNachNummer();

    void berechne();

    int getTime(Category kat);

    static QString getGoodName(QString name); // Wandlet einen Namen in das Format Vorname Nachname um

    int getNextNummer();
    bool checkNummer(int neu);

    static QString getHtmlFuerGesamtuebersicht(QList<Person *> *personen, QList<Category> spalten);
    QString getHtmlFuerEinzelansicht();

public slots:
    void personChangedName(Person *p, QString alt);
    void reloadSettings();

private:
    QSet<Person *> personen;
    QMap<QString, Person*> personenSorted;
    QMap<QString, Person*> idToPerson;

    QMap<Category, int> minimumHours;

    static QMap<Category, int> minimumHoursDefault;

    QMap<Category, int> time;
};

#endif // MANAGERPERSONAL_H
