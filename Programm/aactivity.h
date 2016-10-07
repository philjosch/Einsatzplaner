#ifndef AACTIVITY_H
#define AACTIVITY_H


#include <QDate>
#include <QTime>
#include <QObject>
#include "managerpersonal.h"
//#include "person.h"

class Person;

class AActivity
{

public:
    enum Category { Tf, Tb, Zf, Service, Zub, Begleiter, Buero, Werkstatt, ZugVorbereiten, Sonstiges=100 };

    struct Infos {
        QTime beginn;
        QTime ende;
        Category kategorie;
        QString aufgabe;
        QString bemerkung;
    };

    AActivity(QDate date, ManagerPersonal *p);
    AActivity(QJsonObject o, ManagerPersonal *p);
    ~AActivity();

    bool remove();

    virtual QJsonObject toJson();

    static Category getCategoryFromString(QString s);
    static QString getStringFromCategory(Category c);

    QDate getDatum();
    void setDatum(QDate value);

    QString getOrt() const;
    void setOrt(const QString &value);

    QTime getZeitAnfang();
    void setZeitAnfang(QTime value);

    QTime getZeitEnde();
    void setZeitEnde(QTime value);

    QString getAnlass() const;
    void setAnlass(const QString &value);

    QString getBemerkungen() const;
    void setBemerkungen(const QString &value);

    bool getPersonalBenoetigt() const;
    void setPersonalBenoetigt(bool value);

    QMap<Person *,  Infos*> *getPersonen() const;
    virtual Infos *getIndividual(Person *person) = 0;
    ManagerPersonal::Misstake addPerson(Person *p, QString bemerkung, QTime start, QTime ende, QString aufgabe);
    ManagerPersonal::Misstake addPerson(QString p, QString bemerkung, QTime start, QTime ende, QString aufgabe);
    bool removePerson(Person *p);
    bool removePerson(QString p);
    void setPersonen(QMap<Person *, Infos *> *value);

    virtual QString getListString() = 0;
    virtual QString getListStringShort() = 0;

    ManagerPersonal *getPersonal() const;

    virtual QString getHtmlForSingleView() = 0;
    virtual QString getHtmlForTableView() = 0;

    virtual void emitter() = 0;


protected:
    QDate datum;
    QString ort;
    QTime zeitAnfang;
    QTime zeitEnde;
    QString anlass;
    QString bemerkungen;
    QMap<Person *, Infos*> *personen; // Für Infos siehe oben
    bool personalBenoetigt;

    ManagerPersonal *personal;

    QString listToString(QMap<Person*, Infos*> *liste, QString seperator);
};


#endif // AACTIVITY_H
