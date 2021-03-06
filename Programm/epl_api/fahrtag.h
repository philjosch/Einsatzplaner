#ifndef FAHRTAG_H
#define FAHRTAG_H

#include "reservierung.h"
#include "wagen.h"
#include "basics.h"
#include "aactivity.h"
#include "verteiler.h"

class Fahrtag : public AActivity
{
    Q_OBJECT

public:
    Fahrtag(QDate date, ManagerPersonal *p);
    Fahrtag(QJsonObject o, ManagerPersonal *p);
    ~Fahrtag();

    QJsonObject toJson() const;

    QString getHtmlForSingleView() const;
    QString getHtmlForTableView() const;

    QString getHtmlFuerReservierungsuebersicht() const;

    void setArt(const Art &value);
    Art getArt() const;

    QTime getAnfang(const Category kat) const;
    QTime getZeitTf() const;
    void setZeitTf(QTime value);

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

    int getBelegung(int klasse, int zug = 0) const; // zug == 0: Gesamt
    int getKapazitaet(int klasse) const; //klasse == -1: Gesamt ; klasse == 0: 2.u 3.Klasse

    int getAnzahlReservierungen() const;

    QSet<Reservierung *> getReservierungen() const;

    bool getCheckAll() const;
    void setCheckAll(bool value);

    bool checkPlausibilitaet(QList<int> zuege, QList<int> haltepunkte) const;

public slots:
    QList<Mistake> verteileSitzplaetze();
    bool checkPlaetze(QString p, Reservierung *r) const;

    Reservierung *createReservierung();
    bool removeReservierung(Reservierung *res);


protected:
    bool checkPlaetze(QMap<int, QList<int> > p, Reservierung *r) const;

    QTime zeitTf;
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
