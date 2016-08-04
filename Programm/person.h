#ifndef PERSON_H
#define PERSON_H


#include <QObject>
#include "aactivity.h"

class Person : public QObject
{
    Q_OBJECT

public:
    Person(QString name);

    int getTimeTf();
    int getTimeZf();
    int getTimeZub();
    int getTimeService();
    int getTimeBuero();
    int getTimeWerkstatt();
    int getTimeSum();
    int getSumKilometer();
    int getAnzahl();

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


    QString getName() const;
    void setName(const QString &value);

protected:
    QString vorname;
    QString nachname;
    QString name;
    bool ausbildungTf;
    bool ausbildungZf;
    bool ausbildungRangierer;
    int strecke; // Entfernung vom Wohnort nach Schwarzerden, wird benötigt, um die Kilometer zu berechnen.
    QMap<AActivity*, AActivity::Category> *activities;

private:
    int timeTf;
    int timeZf;
    int timeZub;
    int timeService;
    int timeBuero;
    int timeWerkstatt;
    int timeSum;
    int sumKilometer;
    bool valuesInvalid;
    /* Gibt an, ob die Werte verändert wurden und ob es bemerkt wurde,
     * kann auch sein, dass dies an der Person vorbei passiert ist,
     * in dem eine Veranstlltung verändert wurde,
     * dann muss manuell neuberechnet werden */

signals:
    void nameChanged(Person*, QString);// Person ist die Person und QString gibt den !ALTEN! NAmen an
};

#endif // PERSON_H
