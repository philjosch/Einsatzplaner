#ifndef AACTIVITY_H
#define AACTIVITY_H

#include <QTime>
#include <QComboBox>
#include <QTimeEdit>

#ifndef PERSON_H
class Person;
#endif

#include "managerpersonal.h"

class AActivity
{

public:
    struct Infos {
        QTime beginn;
        QTime ende;
        Category kategorie;
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

    bool getZeitenUnbekannt() const;
    void setZeitenUnbekannt(bool value);

    QString getAnlass() const;
    void setAnlass(const QString &value);

    QString getBemerkungen() const;
    void setBemerkungen(const QString &value);

    bool getPersonalBenoetigt() const;
    void setPersonalBenoetigt(bool value);

    QMap<Person*,  Infos*> getPersonen();
    virtual Infos getIndividual(Person *person) = 0;
    Mistake addPerson(Person *p, QString bemerkung, QTime start, QTime ende, Category kat);
    Mistake addPerson(QString p, QString bemerkung, QTime start, QTime ende, Category kat);
    void updatePersonBemerkung(Person *p, QString bemerkung);
    bool removePerson(Person *p);
    bool removePerson(QString p);

    friend bool operator<(const AActivity &lhs, const AActivity &rhs) { return lhs.lesser(rhs);}
    friend bool operator>(const AActivity &lhs, const AActivity &rhs) { return rhs.lesser(lhs); }
    friend bool operator==(const AActivity &lhs, const AActivity &rhs) { return !(lhs < rhs) && !(rhs < lhs);}
    friend bool operator<=(const AActivity &lhs, const AActivity &rhs) { return !(lhs > rhs);}
    friend bool operator>=(const AActivity &lhs, const AActivity &rhs) { return !(lhs < rhs);}

    virtual QString getKurzbeschreibung() = 0;
    virtual QString getListString() = 0;
    virtual QString getListStringShort() = 0;

    ManagerPersonal *getPersonal() const;

    virtual QString getHtmlForSingleView() = 0;
    virtual QString getHtmlForTableView() = 0;

    virtual void emitter() = 0;
    virtual void deletter() = 0;

    static QComboBox *generateNewCategoryComboBox();
    static QTimeEdit *generateNewTimeEdit();

    static QStringList EXTERNAL_LIST;
    static QStringList QUALIFICATION_LIST;

    static bool hasQualification(Person *p, Category kat, QString bemerkung);
    static bool isExtern(QString bemerkung);

    static void sort(QList<AActivity*> list);

protected:
    QDate datum;
    QString ort;
    QTime zeitAnfang;
    QTime zeitEnde;
    bool zeitenUnbekannt;
    QString anlass;
    QString bemerkungen;
    QMap<Person *, Infos*> personen; // Für Infos siehe oben
    bool personalBenoetigt;

    ManagerPersonal *personal;

    QString listToString(QMap<Person*, Infos*> *liste, QString seperator, bool aufgabe=false);

    static QString COLOR_REQUIRED;
    bool lesser(const AActivity &second) const;

private:
    static void mergeSort(QList<AActivity *> arr, int l, int r);
    static void merge(QList<AActivity *> arr, int l, int m, int r);
};


#endif // AACTIVITY_H
