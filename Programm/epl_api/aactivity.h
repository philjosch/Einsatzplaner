#ifndef AACTIVITY_H
#define AACTIVITY_H

#include "basics.h"

#include <QTime>
#include <QComboBox>
#include <QTimeEdit>

#ifndef PERSON_H
class Person;
#endif

#include "managerpersonal.h"

class AActivity: public QObject
{
    Q_OBJECT

public:
    AActivity(QDate date, ManagerPersonal *p);
    AActivity(QJsonObject o, ManagerPersonal *p);
    virtual ~AActivity();

    virtual Art getArt() const;

    virtual QJsonObject toJson();

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

    bool liegtInVergangenheit();

    QString getAnlass() const;
    void setAnlass(const QString &value);

    QString getBemerkungen() const;
    void setBemerkungen(const QString &value);

    bool getPersonalBenoetigt() const;
    void setPersonalBenoetigt(bool value);

    bool getWichtig() const;
    void setWichtig(bool value);

    bool getAbgesagt() const;
    void setAbgesagt(bool value);

    QList<Einsatz*> getPersonen() const;
    virtual QList<Einsatz> getIndividual(Person *person);
    Einsatz *addPerson(Person *p, QString bemerkung, QTime start, QTime ende, Category kat);
    Einsatz *addPerson(QString p, QString bemerkung, QTime start, QTime ende, Category kat);
    bool removePerson(Einsatz *e);

    static bool lesser(const AActivity *lhs, const AActivity *rhs);
    friend bool operator<(const AActivity &lhs, const AActivity &rhs) { return lesser(&lhs, &rhs);}
    friend bool operator>(const AActivity &lhs, const AActivity &rhs) { return rhs < lhs; }
    friend bool operator==(const AActivity &lhs, const AActivity &rhs) { return (lhs <= rhs) && (lhs >= rhs);}
    friend bool operator<=(const AActivity &lhs, const AActivity &rhs) { return !(lhs > rhs);}
    friend bool operator>=(const AActivity &lhs, const AActivity &rhs) { return !(lhs < rhs);}

    virtual QString getKurzbeschreibung();
    virtual QString getListString();
    virtual QString getListStringShort();

    ManagerPersonal *getPersonal() const;

    virtual QString getHtmlForSingleView();
    virtual QString getHtmlForTableView();

    virtual QString getFarbe();

    static QStringList EXTERNAL_LIST;
    static QStringList QUALIFICATION_LIST;
    static const QString KOPF_LISTE_HTML;
    static const QString FUSS_LISTE_HTML;

    static bool hasQualification(Person *p, Category kat, QString bemerkung, QDate datum = QDate());
    static bool isExtern(QString bemerkung);

    static void sort(QList<AActivity *> *list);

    bool check(Auswahl aus);

signals:
    void changed(AActivity *, QDate = QDate());

protected:
    Art art;
    QDate datum;
    QString ort;
    QTime zeitAnfang;
    QTime zeitEnde;
    bool zeitenUnbekannt;
    QString anlass;
    QString bemerkungen;
    QList<Einsatz*> personen;
    bool personalBenoetigt;
    bool wichtig;
    bool abgesagt;

    ManagerPersonal *personal;

    QString listToString(QString sep, QList<Einsatz*> liste, QString prefix="", QString suffix="", bool aufgabe=false);

    static QString COLOR_REQUIRED;
};

#endif // AACTIVITY_H
