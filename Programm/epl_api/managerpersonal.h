#ifndef MANAGERPERSONAL_H
#define MANAGERPERSONAL_H

#include <QJsonObject>
#include <QObject>
#include <QPrinter>
#include <QSet>

#include "basics.h"
#include "person.h"

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

    void setBeitrag(Person::Beitragsart art, int beitrag);
    int getBeitrag(Person::Beitragsart art) const;

    QList<Person *> getPersonenSortiertNachNummer() const;

    static QString getGoodName(QString name); // Wandlet einen Namen in das Format Vorname Nachname um

    int getNextNummer() const;
    bool checkNummer(int neu) const;

    bool printZeitenEinzel(QList<Person*> liste, Status filter, QPrinter *printer) const;
    static bool printZeitenListe(QList<Person *> personen, QSet<Category> spalten, Status filter, QPrinter *printer);

    bool printMitgliederEinzel(QList<Person*> liste, Status filter, QPrinter *printer) const;
    static bool printMitgliederListe(QList<Person *> liste, Status filter, QSet<QString> data, QPrinter *printer);
    static bool saveMitgliederListeAlsCSV(QList<Person *> liste, QString pfad);

    bool saveBeitraegeRegulaerAlsCSV(QString pfad) const;
    bool saveBeitraegeNachzahlungAlsCSV(QString pfad) const;

    int getAnzahlMitglieder(Status filter) const;
    QList<Person *> getPersonen(Status filter) const;

signals:
    void changed();
    void personChanged(Person*);

private:
    QList<Person *> personen;

    QMap<Category, int> minimumHours;
    QMap<Person::Beitragsart, int> beitraege;

    static const QMap<Category, int> MINIMUM_HOURS_DEFAULT;
    static const QMap<Person::Beitragsart, int> BEITRAEGE_DEFAULT;
};

#endif // MANAGERPERSONAL_H
