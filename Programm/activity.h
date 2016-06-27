#ifndef ACTIVITY_H
#define ACTIVITY_H

#include <QDate>
#include <QTime>
#include "person.h"
#include <QObject>

class Activity
{
public:
    Activity();
    ~Activity();

    static Activity *fromJson(QJsonObject *o);
    QJsonObject *toJson();

    enum Category { Tf, Tb, Zf, Service, Begleiter, Buero, Werkstatt, ZugVorbereiten, Sonstiges=100 };

    QDate *getDatum() const;
    void setDatum(QDate *value);

    QString getOrt() const;
    void setOrt(const QString &value);

    QTime *getZeitAnfang() const;
    void setZeitAnfang(QTime *value);

    QTime *getZeitEnde() const;
    void setZeitEnde(QTime *value);

    QString getAnlass() const;
    void setAnlass(const QString &value);

    QString getBemerkungen() const;
    void setBemerkungen(const QString &value);

    bool getPersonalBenoetigt() const;
    void setPersonalBenoetigt(bool value);

    QMap<Person *, QList<QObject *> *> *getPersonen() const;
    void setPersonen(QMap<Person *, QList<QObject *> *> *value);

private:
    QDate *datum;
    QString ort;
    QTime *zeitAnfang;
    QTime *zeitEnde;
    QString anlass;
    QString bemerkungen;
    QMap<Person *, QList<QObject *> *> *personen;
    bool personalBenoetigt;

signals:
    void activityChanged(Activity*);

};

#endif // ACTIVITY_H
