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
    AActivity(QDate *date, ManagerPersonal *p);
    ~AActivity();

    void fromJson(QJsonObject *o);
    QJsonObject *toJson();

    enum Category { Tf, Tb, Zf, Service, Begleiter, Buero, Werkstatt, ZugVorbereiten, Sonstiges=100 };
    static Category getCategoryFromString(QString s);

    QDate *getDatum() const;
    void setDatum(QDate *value);

    QString getOrt() const;
    void setOrt(const QString &value);

    QTime *getZeitAnfang() const;
    void setZeitAnfang(QTime *value);

    QTime *getZeitEnde() const;
    void setZeitEnde(QTime *value);

    QString getAnlass() const;
    void setAnlass(const QString &value);

    QString getBemerkungen() const;
    void setBemerkungen(const QString &value);

    bool getPersonalBenoetigt() const;
    void setPersonalBenoetigt(bool value);

    QMap<Person *, QList<QObject *> *> *getPersonen() const;
    ManagerPersonal::Misstake addPerson(Person *p, QString s, QTime *start, QTime *ende);
    ManagerPersonal::Misstake addPerson(QString p, QString s, QTime *start, QTime *ende);
    bool removePerson(Person *p);
    bool removePerson(QString p);
    void setPersonen(QMap<Person *, QList<QObject *> *> *value);

    virtual QString getListString() = 0;
    virtual QString getListStringShort() = 0;

protected:
    QDate *datum;
    QString ort;
    QTime *zeitAnfang;
    QTime *zeitEnde;
    QString anlass;
    QString bemerkungen;
    QMap<Person *, QList<QObject *> *> *personen;
    bool personalBenoetigt;

    ManagerPersonal *personal;

private slots:
    virtual void handleActivity(AActivity* a) = 0;
    virtual void handleEmit() = 0;

};


#endif // AACTIVITY_H
