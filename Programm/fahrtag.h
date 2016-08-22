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
    static QString getStringFromArt(Fahrtag::Art art);

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

    QMap<QString, QString> *getListTf() const;
    bool removeTf(QString p);
    bool addTf(QString p, QString bemerkungen);

    QMap<QString, QString> *getListZf() const;
    bool removeZf(QString p);
    bool addZf(QString p, QString bemerkungen);

    QMap<QString, QString> *getListZub() const;
    bool removeZub(QString p);
    bool addZub(QString p, QString bemerkungen);

    QMap<QString, QString> *getListService() const;
    bool removeService(QString p);
    bool addService(QString p, QString bemerkungen);

    QList<int> *getIndividual(Person *person);

signals:
    void fahrtagModified(AActivity *a);

private slots:
    void handleActivity(AActivity *a);
    void handleEmit();

protected:
    /* Fahrtag erweitert personen um ein weiteres Feld:
     * 0: Beginn in msek seit Tagesbeginn
     * 1: Ende in msec seit Tagesbeginn
     * 2: Aufgabe der Person, Integer für AActivity::Category
     * 3: Gibt an, ob die Person in einer anderen liste bereits vorhanden ist.
     *    0: kein andere
     *    1: in Tf vorhanden
     *    2: in Zf vorhanden
     *    3: in zub  --''--
     *    4: in Service --''--
     * */

    QMap<QString, QString> *listTf;
    QMap<QString, QString> *listZf;
    QMap<QString, QString> *listZub;
    QMap<QString, QString> *listService;

    Fahrtag::Art art;
    QTime *zeitTf;
    bool wichtig;
    bool benoetigeTf;
    bool benoetigeZf;
    bool benoetigeZub;
    bool benoetigeService;
};

#endif // FAHRTAG_H
