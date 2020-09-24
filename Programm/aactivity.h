#ifndef AACTIVITY_H
#define AACTIVITY_H

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
    struct Auswahl {
        QString startdate = "tdy";
        QString enddate = "all";
        bool activities = true;

        void insertJson(QJsonObject *o) {
            o->insert("startdate", startdate);
            o->insert("enddate", enddate);
            o->insert("activities", activities);
        }
        static Auswahl fromJson(QJsonObject o) {
            Auswahl a;
            a.startdate = o.value("startdate").toString();
            a.enddate = o.value("enddate").toString();
            a.activities = o.value("activities").toBool(true);
            return a;
        }


        bool check(AActivity *a)
        {
            if (startdate == "tdy") {
                if (a->liegtInVergangenheit()) {
                    return false;
                }
            } else if (startdate == "all") {

            } else if (startdate == "bgn") {
                if (a->getDatum().year() < QDate::currentDate().year()) {
                    return false;
                }
            }
            // Enddatum
            if (enddate == "p1w") {
                QDate ref = QDate::currentDate().addDays(7); // naechste Woche
                ref = ref.addDays(7-ref.dayOfWeek()); // Ende der Woche
                if (a->getDatum() > ref) {
                    return false;
                }
            } else if (enddate == "p1m") {
                QDate ref = QDate::currentDate().addMonths(1); // naechster Monat
                ref = QDate(ref.year(), ref.month(), ref.daysInMonth()); // Ende des Monats
                if (a->getDatum() > ref) {
                    return false;
                }
            } else if (enddate == "eoy") {
                if (a->getDatum().year() > QDate::currentDate().year()) {
                    return false;
                }
            } else if (enddate == "all") {

            }
            // Auch Aktivitaeten?
            if (!activities) {
                if (a->getArt() == Art::Arbeitseinsatz)
                    return false;
            }
            return true;
        }

    };
    struct Einsatz {
        Person *person;
        Category cat;
        bool operator<(const Einsatz &o)  const {
            return (person < o.person || (person == o.person && cat<o.cat));
        }
    };
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

    Person *getPerson(QString name);
    QMap<Einsatz, Infos> getPersonen() const;
    virtual Infos getIndividual(Person *person, Category kat);
    Mistake addPerson(Person *p, QString bemerkung, QTime start, QTime ende, Category kat);
    Mistake addPerson(QString p, QString bemerkung, QTime start, QTime ende, Category kat);
    void updatePersonInfos(Person *p, Category kat, Infos neu);
    void updatePersonBemerkung(Person *p, Category kat, QString bemerkung);
    bool removePerson(Person *p, Category kat);

    friend bool operator<(const AActivity &lhs, const AActivity &rhs) { return lhs.lesser(rhs);}
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

    static QStringList EXTERNAL_LIST;
    static QStringList QUALIFICATION_LIST;

    static bool hasQualification(Person *p, Category kat, QString bemerkung, QDate datum = QDate());
    static bool isExtern(QString bemerkung);

    static void sort(QList<AActivity *> *list);


signals:
    void changed(AActivity *, QDate = QDate());
    void del(AActivity *);

protected:
    QDate datum;
    QString ort;
    QTime zeitAnfang;
    QTime zeitEnde;
    bool zeitenUnbekannt;
    QString anlass;
    QString bemerkungen;
    QMap<Einsatz, Infos> personen;
    bool personalBenoetigt;

    ManagerPersonal *personal;

    QString listToString(QString sep, QMap<Person *, Infos> liste, QString prefix="", QString suffix="", bool aufgabe=false);

    static QString COLOR_REQUIRED;
    bool lesser(const AActivity &second) const;

private:
    static void mergeSort(QList<AActivity *> *arr, int l, int r);
    static void merge(QList<AActivity *> *arr, int l, int m, int r);
};

const QString getFarbe(AActivity *a);

#endif // AACTIVITY_H
