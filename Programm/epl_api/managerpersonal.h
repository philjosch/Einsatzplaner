#ifndef MANAGERPERSONAL_H
#define MANAGERPERSONAL_H

#include <QJsonObject>
#include <QObject>
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
    bool removePerson(Person *p);

    void setMinimumHours(Category cat, int amount);
    int getMinimumHours(Category cat);
    static int getMinimumHoursDefault(Category kat);

    QList<Person *> getPersonenSortiertNachNummer();

    void berechne();

    int getZeiten(Category kat);

    static QString getGoodName(QString name); // Wandlet einen Namen in das Format Vorname Nachname um

    int getNextNummer();
    bool checkNummer(int neu);

    QString getZeitenFuerEinzelListeAlsHTML(QList<Person*> liste, Mitglied filter);
    static QString getZeitenFuerListeAlsHTML(QList<Person *> personen, QSet<Category> spalten, Mitglied filter);

    QString getMitgliederFuerEinzelListeAlsHTML(QList<Person*> liste, Mitglied filter);
    static QString getMitgliederFuerListeAlsHtml(QList<Person *> liste, Mitglied filter);
    static QString getMitgliederFuerListeAlsCSV(QList<Person *> liste);

    int getAnzahlMitglieder(Mitglied filter);
    QList<Person *> getPersonen(Mitglied filter);

public slots:
    void personChangedName(Person *p, QString alt);

signals:
    void changed();

private:
    QSet<Person *> personen;
    QMap<QString, Person*> personenSorted;
    QMap<QString, Person*> idToPerson;

    QMap<Category, int> minimumHours;

    static QMap<Category, int> MINIMUM_HOURS_DEFAULT;

    QMap<Category, int> time;
};

#endif // MANAGERPERSONAL_H
