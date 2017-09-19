#ifndef AACTIVITY_H
#define AACTIVITY_H

#include <QDate>
#include <QTime>
#include <QObject>
#include <QComboBox>
#include <QTimeEdit>

//class AActivity;
//enum Category { Tf, Tb, Zf, Service, Zub, Begleiter, Buero, Werkstatt, ZugVorbereiten, Sonstiges=100 };

#include "managerpersonal.h"

class Person;

class AActivity
{

public:
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
    Misstake addPerson(Person *p, QString bemerkung, QTime start, QTime ende, QString aufgabe);
    Misstake addPerson(QString p, QString bemerkung, QTime start, QTime ende, QString aufgabe);
    void updatePersonBemerkung(Person *p, QString bemerkung);
    bool removePerson(Person *p);
    bool removePerson(QString p);
    void setPersonen(QMap<Person *, Infos *> *value);

    virtual QString getListString() = 0;
    virtual QString getListStringShort() = 0;

    ManagerPersonal *getPersonal() const;

    virtual QString getHtmlForSingleView() = 0;
    virtual QString getHtmlForTableView() = 0;

    virtual void emitter() = 0;

    static QComboBox *generateNewCategoryComboBox();
    static QTimeEdit *generateNewTimeEdit();

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

    QString listToString(QMap<Person*, Infos*> *liste, QString seperator, bool aufgabe=false);

};

#endif // AACTIVITY_H
