#ifndef PERSON_H
#define PERSON_H

#include "aactivity.h"

#include <QMultiMap>

class Person : public QObject
{
    Q_OBJECT

public:
    Person(QString name, ManagerPersonal *manager);
    Person(QJsonObject o, ManagerPersonal *man); // Laden aus einem JSON-Objekt

    static const QString FARBE_FEHLENDE_STUNDEN;
    static const QString FARBE_GENUG_STUNDEN;
    static const QString FARBE_STANDARD;
    static const QString KOPF_TABELLE_LISTE_CSV;
    static const QString KOPF_TABELLE_LISTE_HTML;
    static const QString FUSS_TABELLE_LISTE_HTML;

    QJsonObject toJson();
    QJsonObject personalToJson();

    int getZeiten(Category cat);
    Mitglied pruefeStunden();
    Mitglied pruefeStunden(Category cat);

    int getMinimumStunden(Category cat);


    void berechne();

    bool addActivity(AActivity *a, Category kat);
    bool removeActivity(AActivity *a, Category kat);

    QMultiMap<AActivity*,Category> getActivities();


    QString getZeitenFuerListeAlsHTML(QSet<Category> liste);
    QString getZeitenFuerEinzelAlsHTML();

    QString getPersonaldatenFuerListeAlsHTML();
    QString getPersonaldatenFuerListeAlsCSV();
    QString getPersonaldatenFuerEinzelAlsHTML();


    int getAdditional(Category cat);
    void setAdditional(Category cat, int value);


    QString getId() const;

    int getNummer() const;
    bool setNummer(int value);
    int setAutoNummer();

    QString getName() const;

    QString getVorname() const;
    bool setVorname(const QString &value);

    QString getNachname() const;
    bool setNachname(const QString &value);

    QDate getGeburtstag() const;
    void setGeburtstag(const QDate &value);
    bool isMinderjaehrig();

    QDate getEintritt() const;
    void setEintritt(const QDate &value);

    bool getAktiv() const;
    void setAktiv(bool value);

    QDate getAustritt() const;
    void setAustritt(const QDate &value);

    bool getAusbildungTf() const;
    void setAusbildungTf(bool value);

    bool getAusbildungZf() const;
    void setAusbildungZf(bool value);

    bool getAusbildungRangierer() const;
    void setAusbildungRangierer(bool value);

    QDate getTauglichkeit() const;
    void setTauglichkeit(const QDate &value);

    QString getMail() const;
    void setMail(const QString &value);

    bool getMailOK() const;
    void setMailOK(bool value);

    QString getTelefon() const;
    void setTelefon(const QString &value);

    bool getTelefonOK() const;
    void setTelefonOK(bool value);

    int getStrecke() const;
    void setStrecke(int value);

    QString getBeruf() const;
    void setBeruf(const QString &value);

    QString getBemerkungen() const;
    void setBemerkungen(const QString &value);

    bool isAusgetreten();

    QString getPLZ() const;
    void setPLZ(const QString &value);

    QString getOrt() const;
    void setOrt(const QString &value);

    QString getStrasse() const;
    void setStrasse(const QString &value);

    bool isTauglich(Category cat=Tf, QDate datum=QDate::currentDate());

    bool pruefeFilter(Mitglied filter);

    ManagerPersonal *getManager() const;

protected:
    // Stammdaten
    QString id;
    int nummer;
    QString vorname;
    QString nachname;
    QDate geburtstag;
    QDate eintritt;
    bool aktiv;
    QDate austritt;
    // Betriebsdienst
    bool ausbildungTf;
    bool ausbildungZf;
    bool ausbildungRangierer;
    QDate tauglichkeit;
    // Kontakt
    QString plz;
    QString ort;
    QString strasse;
    QString mail;
    bool mailOK;
    QString telefon;
    bool telefonOK;
    // Sonstiges
    int strecke;
    QString beruf;
    QString bemerkungen;
    // Zusätzliche Stunden
    QMap<Category, int> additional;

    QMap<Category, int> zeiten;
    QMultiMap<AActivity*, Category> activities;

private:
    void personConstructor(QString vn, QString nn, ManagerPersonal *man, QString ID="");

    bool valuesInvalid;
    /* Gibt an, ob die Werte verändert wurden und ob es bemerkt wurde,
     * kann auch sein, dass dies an der Person vorbei passiert ist,
     * in dem eine Veranstalltung verändert wurde,
     * dann muss manuell neuberechnet werden */

    ManagerPersonal *manager;

signals:
    void changed();
    void nameChanged(Person*, QString);// Person ist die Person und QString gibt den !ALTEN! Namen an
    void del(Person *);
};

#endif // PERSON_H
