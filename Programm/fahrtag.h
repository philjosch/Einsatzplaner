#ifndef FAHRTAG_H
#define FAHRTAG_H

#include "aactivity.h"
#include "managerreservierungen.h"

class Fahrtag : public QObject, public AActivity, public ManagerReservierungen
{
    Q_OBJECT

public:
    Fahrtag(QDate *date, ManagerPersonal *p);
    ~Fahrtag();

    enum Art { Museumszug, Sonderzug, Gesellschaftssonderzug, Nikolauszug, Schnupperkurs, ELFundMuseumszug, Bahnhofsfest, Sonstiges=100 };
    static QString getStringFromArt(Fahrtag::Art art);

    QString getListString();
    QString getListStringShort();

    void setArt(const Fahrtag::Art &value);
    Fahrtag::Art getArt() const;

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

    AActivity::Infos *getIndividual(Person *person);

    QString getHtmlForSingleView();
    QString getHtmlForTableView();


signals:
    void fahrtagModified(AActivity *a);

private slots:
    void handleActivity(AActivity *a);
    void handleEmit();

protected:
    Fahrtag::Art art;
    QTime *zeitTf;
    bool wichtig;
    bool benoetigeTf;
    bool benoetigeZf;
    bool benoetigeZub;
    bool benoetigeService;
};

#endif // FAHRTAG_H
