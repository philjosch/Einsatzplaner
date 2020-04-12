#ifndef FAHRTAG_H
#define FAHRTAG_H

#include "aactivity.h"
#include "managerreservierungen.h"

class Fahrtag : public AActivity, public ManagerReservierungen
{
    Q_OBJECT

public:
    Fahrtag(QDate date, ManagerPersonal *p);
    Fahrtag(QJsonObject o, ManagerPersonal *p);
    ~Fahrtag();

    QJsonObject toJson();

    static QString getStringFromArt(Art art);

    QString getKurzbeschreibung();
    QString getListString();
    QString getListStringShort();

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

    Infos getIndividual(Person *person);

    QString getHtmlForSingleView();
    QString getHtmlForTableView();

    QString getHtmlFuerReservierungsuebersicht();

protected:
    Art art;
    QTime zeitTf;
    bool wichtig;
    int benoetigeTf;
    bool benoetigeZf;
    bool benoetigeZub;
    bool benoetigeService;
};

#endif // FAHRTAG_H
