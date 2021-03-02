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
        AnfangBedingung startdate = AbJetzt;
        EndeBedingung enddate = BisAlle;
        bool activities = true;

        void insertJson(QJsonObject *o) {
            o->insert("startdate", zuString(startdate));
            o->insert("enddate", zuString(enddate));
            o->insert("activities", activities);
        }
        static Auswahl fromJson(QJsonObject o) {
            Auswahl a;
            a.startdate = anfangAusString(o.value("startdate").toString());
            a.enddate = endeAusString(o.value("enddate").toString());
            a.activities = o.value("activities").toBool(true);
            return a;
        }
        static AnfangBedingung anfangAusString(QString s) {
            if (s == "tdy") {
                return AbJetzt;
            }
            if (s == "all") {
                return AbAlle;
            }
            if (s == "bgn") {
                return AbAnfangDesJahres;
            }
            return AbAlle;
        }
        static QString zuString(AnfangBedingung a) {
            switch (a) {
            case AbJetzt:
                return "tdy";
            case AbAlle:
                return "all";
            case AbAnfangDesJahres:
                return "bgn";
            default:
                return "all";
            }
        }

        static EndeBedingung endeAusString(QString s) {
            if (s == "p1w") {
                return BisEndeNaechsterWoche;
            }
            if (s == "p1m") {
                return BisEndeNaechsterMonat;
            }
            if (s == "eoy") {
                return BisEndeDesJahres;
            }
            if (s == "all") {
                return BisAlle;
            }
            return BisAlle;
        }
        static QString zuString(EndeBedingung e) {
            switch (e) {
            case BisEndeDesJahres:
                return "eoy";
            case BisAlle:
                return "all";
            case BisEndeNaechsterWoche:
               return "p1w";
            case BisEndeNaechsterMonat:
                return "p1m";
            default:
                return "all";
            }
        }

        bool check(AActivity *a)
        {
            switch (startdate) {
            case AbJetzt:
                if (a->liegtInVergangenheit()) {
                    return false;
                }
                break;
            case AbAlle:
                break;
            case AbAnfangDesJahres:
                if (a->getDatum().year() < QDate::currentDate().year()) {
                    return false;
                }
                break;
            default:
                break;
            }
            // Enddatum
            QDate ref;
            switch (enddate) {
            case BisAlle:
                break;
            case BisEndeNaechsterWoche:
                ref = QDate::currentDate().addDays(7); // naechste Woche
                ref = ref.addDays(7-ref.dayOfWeek()); // Ende der Woche
                if (a->getDatum() > ref) {
                    return false;
                }
                break;
            case BisEndeNaechsterMonat:
                ref = QDate::currentDate().addMonths(1); // naechster Monat
                ref = QDate(ref.year(), ref.month(), ref.daysInMonth()); // Ende des Monats
                if (a->getDatum() > ref) {
                    return false;
                }
            case BisEndeDesJahres:
                if (a->getDatum().year() > QDate::currentDate().year()) {
                    return false;
                }
            default:
                break;
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

    bool getWichtig() const;
    void setWichtig(bool value);

    bool getAbgesagt() const;
    void setAbgesagt(bool value);

    Person *getPerson(QString name);
    QMap<Einsatz, Infos> getPersonen() const;
    virtual Infos getIndividual(Person *person, Category kat);
    Einsatz addPerson(Person *p, QString bemerkung, QTime start, QTime ende, Category kat);
    Einsatz addPerson(QString p, QString bemerkung, QTime start, QTime ende, Category kat);
    void updatePersonInfos(Person *p, Category kat, Infos neu);
    void updatePersonBemerkung(Person *p, Category kat, QString bemerkung);
    bool removePerson(Person *p, Category kat);

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
    QMap<Einsatz, Infos> personen;
    bool personalBenoetigt;
    bool wichtig;
    bool abgesagt;

    ManagerPersonal *personal;

    QString listToString(QString sep, QMap<Person *, Infos> liste, QString prefix="", QString suffix="", bool aufgabe=false);

    static QString COLOR_REQUIRED;
};

#endif // AACTIVITY_H
