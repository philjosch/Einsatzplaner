#ifndef MANAGERPERSONAL_H
#define MANAGERPERSONAL_H

#include <QJsonObject>
#include <QObject>
#include <QSet>

#include "basics.h"

class Person;

class ManagerPersonal :  public QObject
{
    Q_OBJECT

public:
    ManagerPersonal();
    ManagerPersonal(QJsonObject o);
    ~ManagerPersonal();

    QJsonObject toJson() const;
    QJsonObject personalToJson() const;

    Person *getPersonFromID(QString id) const;
    Person *getPerson(QString name) const;
    bool personExists(QString name) const; // Prüft, ob die Person im System registriert ist
    bool personExists(QString vorname, QString nachname) const; // Prüft, ob die Person im System registriert ist

    Person *newPerson(); // Erstellt eine neue Person und gibt sie zurück
    bool removePerson(Person *p);

    void setMinimumHours(Category cat, int amount);
    int getMinimumHours(Category cat) const;
    static int getMinimumHoursDefault(Category kat);

    QList<Person *> getPersonenSortiertNachNummer() const;

    static QString getGoodName(QString name); // Wandlet einen Namen in das Format Vorname Nachname um

    int getNextNummer() const;
    bool checkNummer(int neu) const;

    QString getZeitenFuerEinzelListeAlsHTML(QList<Person*> liste, Status filter) const;
    static QString getZeitenFuerListeAlsHTML(QList<Person *> personen, QSet<Category> spalten, Status filter);

    QString getMitgliederFuerEinzelListeAlsHTML(QList<Person*> liste, Status filter) const;
    static QString getMitgliederFuerListeAlsHtml(QList<Person *> liste, Status filter, QSet<QString> data);
    static QString getMitgliederFuerListeAlsCSV(QList<Person *> liste);

    int getAnzahlMitglieder(Status filter) const;
    QList<Person *> getPersonen(Status filter) const;

signals:
    void changed();
    void personChanged(Person*);

private:
    QList<Person *> personen;

    QMap<Category, int> minimumHours;

    static const QMap<Category, int> MINIMUM_HOURS_DEFAULT;
};

#endif // MANAGERPERSONAL_H
