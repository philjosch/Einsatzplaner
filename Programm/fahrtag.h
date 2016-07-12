#ifndef FAHRTAG_H
#define FAHRTAG_H

#include "aactivity.h"
#include "managerreservierungen.h"

class Fahrtag : public ManagerReservierungen, public AActivity
{
    Q_OBJECT

public:
    Fahrtag(QDate *date);

    enum Art { Museumszug, Sonderzug, Nikolauszug, Schnupperkurs, ELFundMuseumszug, Bahnhofsfest, Sonstiges=100 };

    QString getListString();
    QString getListStringShort();

    Fahrtag::Art getArt() const;
    void setArt(const Fahrtag::Art &value);

signals:
    void fahrtagModified(AActivity *a);
    void activityChanged(AActivity *a);

public slots:
    void handleActivity(AActivity *a);

private:
    Fahrtag::Art art;
};

#endif // FAHRTAG_H
