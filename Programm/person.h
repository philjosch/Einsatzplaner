#ifndef PERSON_H
#define PERSON_H

#include "aactivity.h"

class Person : public QObject
{
    Q_OBJECT

public:
    Person(QString vorname, QString nachname, ManagerPersonal *manager);
    Person(QString name, ManagerPersonal *manager);

    QJsonObject toJson();
    QJsonObject personalToJson();
    static Person *fromJson(QJsonObject o, ManagerPersonal *manager);

    double getTime(Category cat);
    double getTimeTf();
    double getTimeZf();
    double getTimeZub();
    double getTimeService();
    double getTimeBuero();
    double getTimeWerkstatt();
    double getTimeVorbereiten();
    double getTimeAusbildung();
    double getTimeSonstiges();
    double getTimeSum();

    int getAnzahl();
    int getSumAnzahl();
    double getSumKilometer();

    bool getAusbildungTf() const;
    void setAusbildungTf(bool value);

    bool getAusbildungZf() const;
    void setAusbildungZf(bool value);

    bool getAusbildungRangierer() const;
    void setAusbildungRangierer(bool value);

    int getStrecke() const;
    void setStrecke(int value);

    void berechne();

    bool addActivity(AActivity *a, Category category);
    bool removeActivity(AActivity *a);

    QListIterator<AActivity *> *getActivities();


    QString getName() const;
    QString getVorname() const;
    void setVorname(const QString &value);
    QString getNachname() const;
    void setNachname(const QString &value);

    QString getHtmlForTableView(QList<Category> liste);
    QString getHtmlForDetailPage(ManagerPersonal *m);

    double getAdditionalTimeTf() const;
    void setAdditionalTimeTf(double value);
    double getAdditionalTimeZf() const;
    void setAdditionalTimeZf(double value);
    double getAdditionalTimeZub() const;
    void setAdditionalTimeZub(double value);
    double getAdditionalTimeService() const;
    void setAdditionalTimeService(double value);
    double getAdditionalTimeBuero() const;
    void setAdditionalTimeBuero(double value);
    double getAdditionalTimeWerkstatt() const;
    void setAdditionalTimeWerkstatt(double value);
    double getAdditionalTimeVorbereiten() const;
    void setAdditionalTimeVorbereiten(double value);
    double getAdditionalTimeAusbildung() const;
    void setAdditionalTimeAusbildung(double value);
    double getAdditionalTimeSonstiges() const;
    void setAdditionalTimeSonstiges(double value);

    int getAdditionalAnzahl() const;
    void setAdditionalAnzahl(int value);
    double getAdditionalKilometer() const;
    void setAdditionalKilometer(double value);


protected:
    QString vorname;
    QString nachname;
    bool ausbildungTf;
    bool ausbildungZf;
    bool ausbildungRangierer;
    int strecke; // Entfernung vom Wohnort nach Schwarzerden, wird benötigt, um die Kilometer zu berechnen.
    QMap<AActivity*, Category> activities;

private:
    void personConstructor(QString vorname, QString nachname, ManagerPersonal *manager);

    double timeTf;
    double timeZf;
    double timeZub;
    double timeService;
    double timeBuero;
    double timeWerkstatt;
    double timeVorbereiten;
    double timeAusbildung;
    double timeSonstiges;
    double timeSum;

    int sumAnzahl;
    double sumKilometer;

    bool valuesInvalid;
    /* Gibt an, ob die Werte verändert wurden und ob es bemerkt wurde,
     * kann auch sein, dass dies an der Person vorbei passiert ist,
     * in dem eine Veranstlltung verändert wurde,
     * dann muss manuell neuberechnet werden */

    double additionalTimeTf;
    double additionalTimeZf;
    double additionalTimeZub;
    double additionalTimeService;
    double additionalTimeBuero;
    double additionalTimeWerkstatt;
    double additionalTimeVorbereiten;
    double additionalTimeAusbildung;
    double additionalTimeSonstiges;

    int additionalAnzahl;
    double additionalKilometer;

    ManagerPersonal *manager;

signals:
    void nameChanged(Person*, QString);// Person ist die Person und QString gibt den !ALTEN! Namen an
};

#endif // PERSON_H
