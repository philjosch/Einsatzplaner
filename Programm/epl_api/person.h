#ifndef PERSON_H
#define PERSON_H

#include "einsatz.h"
#include "export.h"

#include <QMultiMap>
#include <QValidator>

class ManagerPersonal;

class IbanValidator : public QValidator {

public:
    IbanValidator() {};

    QValidator::State validate(QString &input, int &pos) const;
};

class Person : public QObject
{
    Q_OBJECT

public:

    enum Geschlecht {
        GeschlechtUnbekannt =  0,
        Juristisch          =  1,
        Maennlich           = 10,
        Weiblich            = 11,
        Divers              = 12
    };
    static QString toString(Geschlecht g);
    enum Beitragsart {
        BeitragUnbekannt      = 0,
        Beitragsfrei          = 1,
        AktivenBeitrag        = 2,
        FoerderBeitrag        = 3,
        FamilienBeitragZahler = 4,
        FamilienBeitragNutzer = 5,
        Ermaessigt            = 6
    };
    static QString toString(Beitragsart b);

    Person(QString name, ManagerPersonal *man);
    Person(QJsonObject o, ManagerPersonal *man);

    static bool lessByNumber(const Person * const &lhs, const Person * const &rhs);
    static bool lessByName(const Person * const &lhs, const Person * const &rhs);
    friend bool operator<(const Person &lhs, const Person &rhs) {return lessByName(&lhs, &rhs);};

    static const QString FARBE_FEHLENDE_STUNDEN;
    static const QString FARBE_GENUG_STUNDEN;
    static const QString FARBE_STANDARD;
    static QString getKopfTabelleListeHtml(QSet<QString> data);
    static const QString FUSS_TABELLE_LISTE_HTML;
    static const QStringList ANZEIGE_PERSONALDATEN;

    QJsonObject toJson() const;
    QJsonObject personalToJson() const;

    int getZeiten(Category cat) const;
    Status pruefeStunden() const;
    Status pruefeStunden(Category cat) const;

    int getMinimumStunden(Category cat) const;

    bool pruefeFilter(Status filter);

    void berechne() const;

    void addActivity(Einsatz *e);
    bool removeActivity(Einsatz *e);

    const QList<Einsatz *> getActivities() const;


    QString getZeitenFuerListeAlsHTML(QSet<Category> liste) const;
    QString getZeitenFuerEinzelAlsHTML() const;
    bool exportTimesAsHtml(ExportHtml *printer) const;

    QString getPersonaldatenFuerListeAlsHTML(QSet<QString> anzeige) const;
    QString getPersonaldatenFuerListeAlsCSV(QStringList attributesForExport) const;
    QString getPersonaldatenFuerEinzelAlsHTML() const;
    bool exportMemberdataAsHtml(ExportHtml *printer) const;

    int getAdditional(Category cat) const;
    void setAdditional(Category cat, int value);

    // Stammdaten
    QString getId() const;

    QString getVorname() const;
    bool setVorname(const QString &value);

    QString getNachname() const;
    bool setNachname(const QString &value);

    QString getName() const;
    QString getFullCanonicalName() const;
    QString getNameSortierung() const;

    QDate getGeburtstag() const;
    void setGeburtstag(const QDate &value);
    bool isMinderjaehrig() const;

    QString getAnrede() const;
    void setAnrede(const QString &value);

    Geschlecht getGeschlecht() const;
    void setGeschlecht(const Geschlecht &value);

    QString getBeruf() const;
    void setBeruf(const QString &value);

    // Mitgliedschaft
    int getNummer() const;
    bool setNummer(int value);
    int setAutoNummer();

    QDate getEintritt() const;
    void setEintritt(const QDate &value);

    bool getAktiv() const;
    void setAktiv(bool value);

    QDate getAustritt() const;
    void setAustritt(const QDate &value);
    bool isAusgetreten() const;

    Beitragsart getBeitragsart() const;
    void setBeitragsart(const Beitragsart &value);
    int getBeitrag() const;

    QString getIbanFinal() const;
    QString getIban() const;
    void setIban(const QString &value);

    QString getBankFinal() const;
    QString getBank() const;
    void setBank(const QString &value);

    QString getKontoinhaberFinal() const;
    QString getKontoinhaber() const;
    Person *getKontoinhaberPerson() const;
    void setKontoinhaber(const QString &value);

    int getBeitragRegulaerIndividuell() const;
    int getBeitragNachzahlung() const;

    // Ausbildung
    bool getAusbildungTf() const;
    void setAusbildungTf(bool value);

    bool getAusbildungZf() const;
    void setAusbildungZf(bool value);

    bool getAusbildungRangierer() const;
    void setAusbildungRangierer(bool value);

    QDate getTauglichkeit() const;
    void setTauglichkeit(const QDate &value);
    bool isTauglich(Category cat=Tf, QDate datum=QDate::currentDate()) const;

    QString getSonstigeBetrieblich() const;
    void setSonstigeBetrieblich(const QString &value);

    QString getSonstigeAusbildung() const;
    void setSonstigeAusbildung(const QString &value);

    // Adresse
    QString getPLZ() const;
    void setPLZ(const QString &value);

    QString getOrt() const;
    void setOrt(const QString &value);

    QString getStrasse() const;
    void setStrasse(const QString &value);

    int getStrecke() const;
    void setStrecke(int value);

    // Kontakt
    QString getMail() const;
    void setMail(const QString &value);

    QString getTelefon() const;
    void setTelefon(const QString &value);

    QString getTelefon2() const;
    void setTelefon2(const QString &value);

    // Datenschutz
    bool getMailOK() const;
    void setMailOK(bool value);

    bool getTelefonOK() const;
    void setTelefonOK(bool value);

    // Sonstiges
    QString getBemerkungen() const;
    void setBemerkungen(const QString &value);

protected:
    // Stammdaten
    QString id;

    QString vorname;
    QString nachname;
    QDate geburtstag;
    QString anrede;
    Geschlecht geschlecht;
    QString beruf;

    // Mitgliedschaft
    int nummer;
    QDate eintritt;
    bool aktiv;
    QDate austritt;
    Beitragsart beitragsart;
    QString iban;
    QString bank;
    QString kontoinhaber;
    // Ausbildung
    bool ausbildungTf;
    bool ausbildungZf;
    bool ausbildungRangierer;
    QDate tauglichkeit;
    QString sonstigeBetrieblich;
    QString sonstigeAusbildung;

    // Kontakt
    QString plz;
    QString ort;
    QString strasse;
    int strecke;
    QString mail;
    QString telefon;
    QString telefon2;
    // Datenschutz
    bool mailOK;
    bool telefonOK;
    // Sonstiges
    QString bemerkungen;
    // Zusätzliche Stunden
    QMap<Category, int> additional;

    mutable QMap<Category, int> zeiten;
    mutable QList<Einsatz*> activities;

private:
    mutable bool valuesInvalid;
    /* Gibt an, ob die Werte verändert wurden und ob es bemerkt wurde,
     * kann auch sein, dass dies an der Person vorbei passiert ist,
     * in dem eine Veranstaltung verändert wurde,
     * dann muss manuell neuberechnet werden */

    ManagerPersonal *manager;

    static QString getRandomID();

    static void anfuegen(QString *zelle, QString appendix, QString seperator = "<br/>");

signals:
    void changed();
    void nameChanged(Person*, QString);// Person ist die Person und QString gibt den !ALTEN! Namen an
};

#endif // PERSON_H
