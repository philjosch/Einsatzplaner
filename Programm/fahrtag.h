#ifndef FAHRTAG_H
#define FAHRTAG_H

#include "reservierung.h"
#include "wagen.h"
#include "basics.h"
#include "aactivity.h"

class Fahrtag : public AActivity
{
    Q_OBJECT

public:
    Fahrtag(QDate date, ManagerPersonal *p);
    Fahrtag(QJsonObject o, ManagerPersonal *p);
    ~Fahrtag();

    QJsonObject toJson();

    QString getKurzbeschreibung();
    QString getListString();
    QString getListStringShort();

    QString getHtmlForSingleView();
    QString getHtmlForTableView();

    QString getHtmlFuerReservierungsuebersicht();


    void setArt(const Art &value);
    Art getArt() const;

    QTime getZeitTf();
    void setZeitTf(QTime value);

    bool getWichtig() const;
    void setWichtig(bool value);

    int getBenoetigeTf() const;
    void setBenoetigeTf(int value);

    bool getBenoetigeZf() const;
    void setBenoetigeZf(bool value);

    bool getBenoetigeZub() const;
    void setBenoetigeZub(bool value);

    bool getBenoetigeService() const;
    void setBenoetigeService(bool value);

    QString getWagenreihung() const;
    bool setWagenreihung(const QString &value);

    int getBelegung(int klasse, int zug = 0); // zug == 0: Gesamt
    int getKapazitaet(int klasse); //klasse == -1: Gesamt ; klasse == 0: 2.u 3.Klasse

    int getAnzahl();

    QSet<Reservierung *> getReservierungen();

    bool getCheckAll() const;
    void setCheckAll(bool value);

    bool checkPlausibilitaet(QList<int> zuege, QList<int> haltepunkte);

    Infos getIndividual(Person *person);

public slots:
    QList<Mistake> verteileSitzplaetze();
    bool checkPlaetze(QString p, Reservierung *r);

    Reservierung *createReservierung();
    bool removeReservierung(Reservierung *res);


protected:
    bool checkPlaetze(QMap<int, QList<int> > p, Reservierung *r);

    Art art;
    QTime zeitTf;
    bool wichtig;
    int benoetigeTf;
    bool benoetigeZf;
    bool benoetigeZub;
    bool benoetigeService;
    QString wagenreihung;
    QSet<Reservierung *> reservierungen;
    bool checkAll;

    QList<Wagen *> wagen;
    QMap<int, Wagen *> nummerToWagen;

    bool createWagen();

};

#endif // FAHRTAG_H
