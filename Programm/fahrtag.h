#ifndef FAHRTAG_H
#define FAHRTAG_H

#include "aactivity.h"
#include "managerreservierungen.h"

class Fahrtag : public ManagerReservierungen, public AActivity
{
    Q_OBJECT

public:
    Fahrtag(QDate *date, ManagerPersonal *p);

    enum Art { Museumszug, Sonderzug, Nikolauszug, Schnupperkurs, ELFundMuseumszug, Bahnhofsfest, Sonstiges=100 };

    QString getListString();
    QString getListStringShort();

    Fahrtag::Art getArt() const;
    void setArt(const Fahrtag::Art &value);

    QTime *getZeitTf() const;
    void setZeitTf(QTime *value);

    bool getWichtig() const;
    void setWichtig(bool value);

    bool getBenoetigeTf() const;
    void setBenoetigeTf(bool value);

    bool getBenoetigeZf() const;
    void setBenoetigeZf(bool value);

    bool getBenoetigeZub() const;
    void setBenoetigeZub(bool value);

    bool getBenoetigeService() const;
    void setBenoetigeService(bool value);

signals:
    void fahrtagModified(AActivity *a);

private slots:
    void handleActivity(AActivity *a);
    void handleEmit();

protected:
    /*
listTf
listZf
listeZub
listeService
*/
    Fahrtag::Art art;
    QTime *zeitTf;
    bool wichtig;
    bool benoetigeTf;
    bool benoetigeZf;
    bool benoetigeZub;
    bool benoetigeService;
};

#endif // FAHRTAG_H
