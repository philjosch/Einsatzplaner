#ifndef AACTIVITY_H
#define AACTIVITY_H

#include "basics.h"
#include "einsatz.h"
#include "auswahl.h"
#include "managerpersonal.h"

#include <QTime>
#include <QComboBox>
#include <QTimeEdit>


class AActivity: public QObject
{
    Q_OBJECT

public:
    AActivity(QDate date, ManagerPersonal *p);
    AActivity(QJsonObject o, ManagerPersonal *p);
    virtual ~AActivity();

    virtual Art getArt() const;

    virtual QJsonObject toJson() const;

    QDate getDatum() const;
    void setDatum(QDate value);

    QString getOrt() const;
    void setOrt(const QString &value);

    QTime getZeitAnfang() const;
    virtual QTime getAnfang(const Category kat) const;
    void setZeitAnfang(QTime value);

    QTime getZeitEnde() const;
    virtual QTime getEnde(const Category kat) const;
    void setZeitEnde(QTime value);

    bool getZeitenUnbekannt() const;
    void setZeitenUnbekannt(bool value);

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

    bool check(Auswahl aus) const;

    QList<Einsatz*> getPersonen() const;
    Einsatz *addPerson(QString p, QString bemerkung, Category kat);
    bool removePerson(Einsatz *e);

    static bool lesser(const AActivity *lhs, const AActivity *rhs);
    friend bool operator<(const AActivity &lhs, const AActivity &rhs) { return lesser(&lhs, &rhs);}
    friend bool operator>(const AActivity &lhs, const AActivity &rhs) { return rhs < lhs; }
    friend bool operator==(const AActivity &lhs, const AActivity &rhs) { return (lhs <= rhs) && (lhs >= rhs);}
    friend bool operator<=(const AActivity &lhs, const AActivity &rhs) { return !(lhs > rhs);}
    friend bool operator>=(const AActivity &lhs, const AActivity &rhs) { return !(lhs < rhs);}

    virtual QString getString() const;
    virtual QString getStringShort() const;

    virtual QString getHtmlForSingleView() const;
    virtual QString getHtmlForTableView() const;

    virtual QString getFarbe() const;

    static const QString KOPF_LISTE_HTML;
    static const QString FUSS_LISTE_HTML;

    static void sort(QList<AActivity *> *list);

signals:
    void changed(AActivity *, QDate = QDate());

protected:
    static const QStringList EXTERNAL_LIST;
    static const QStringList QUALIFICATION_LIST;

    static const QString COLOR_REQUIRED;
    static const QMap<Art, QString> FARBE_FAHRTAGE;

    static bool hasQualification(Person *p, Category kat, QString bemerkung, QDate datum = QDate());
    static bool isExtern(QString bemerkung);

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

    QString listToString(QString sep, QList<Einsatz*> liste, QString prefix="", QString suffix="", bool aufgabe=false) const;

    QDateTime getAnfangGenau() const;
    QDateTime getEndeGenau() const;

    QMap<Category, QList<Einsatz *> *> splitNachKategorie() const;
};

#endif // AACTIVITY_H
