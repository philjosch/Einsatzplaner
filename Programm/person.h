#ifndef PERSON_H
#define PERSON_H

#include <QObject>
#include <QVariant>

#include "aactivity.h"

class Person : public QObject
{
    Q_OBJECT

public:
    Person(QString name);

    QJsonObject toJson();
    static Person *fromJson(QJsonObject o);

    double getTimeTf();
    double getTimeZf();
    double getTimeZub();
    double getTimeService();
    double getTimeBuero();
    double getTimeWerkstatt();
    double getTimeVorbereiten();
    double getTimeSonstiges();
    double getTimeSum();
    double getSumKilometer();
    double getAnzahl();

    bool getAusbildungTf() const;
    void setAusbildungTf(bool value);

    bool getAusbildungZf() const;
    void setAusbildungZf(bool value);

    bool getAusbildungRangierer() const;
    void setAusbildungRangierer(bool value);

    int getStrecke() const;
    void setStrecke(int value);

    void berechne();

    bool addActivity(AActivity *a, AActivity::Category category);
    bool removeActivity(AActivity *a);

    QListIterator<AActivity *> *getActivities();


    QString getName() const;
    QString getVorname() const;
    QString getNachname() const;
    void setName(const QString &value);

    QString getHtmlForTableView(QList<bool> *liste);

protected:
    QString name;
    bool ausbildungTf;
    bool ausbildungZf;
    bool ausbildungRangierer;
    int strecke; // Entfernung vom Wohnort nach Schwarzerden, wird benötigt, um die Kilometer zu berechnen.
    QMap<AActivity*, AActivity::Category> *activities;

private:
    double timeTf;
    double timeZf;
    double timeZub;
    double timeService;
    double timeBuero;
    double timeWerkstatt;
    double timeVorbereiten;
    double timeSonstiges;
    double timeSum;
    double sumKilometer;
    bool valuesInvalid;
    /* Gibt an, ob die Werte verändert wurden und ob es bemerkt wurde,
     * kann auch sein, dass dies an der Person vorbei passiert ist,
     * in dem eine Veranstlltung verändert wurde,
     * dann muss manuell neuberechnet werden */

signals:
    void nameChanged(Person*, QString);// Person ist die Person und QString gibt den !ALTEN! NAmen an
};

#endif // PERSON_H
