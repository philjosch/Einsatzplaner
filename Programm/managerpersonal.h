#ifndef MANAGERPERSONAL_H
#define MANAGERPERSONAL_H

#include <QJsonObject>
#include <QObject>
#include <QSet>

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

    enum Misstake { OK, PersonNichtGefunden, FalscheQualifikation, SonstigerFehler};

    Person *getPerson(QString name);
    bool personExists(QString name); // Prüft, ob die Person im System registriert ist
    bool personExists(QString vorname, QString nachname); // Prüft, ob die Person im System registriert ist

    Person *registerPerson(QString vorname, QString nachname); // Fügt die Person in das System ein
    Person *registerPerson(QString name); // Fügt die Person in das System ein
    bool removePerson(QString name);
    bool removePerson(Person *p);

    static bool pruefeStunden(Person *p);

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
};

#endif // MANAGERPERSONAL_H
