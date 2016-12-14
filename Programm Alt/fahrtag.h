#ifndef FAHRTAG_H
#define FAHRTAG_H

#include <QObject>
#include <QDate>
#include "managerreservierungen.h"


class Fahrtag
{
public:
    Fahrtag(QListWidgetItem *item, QListWidget *liste);
    ~Fahrtag();

private:
    QListWidgetItem *item;
    QListWidget *liste;
    QDate datum;
    int art;
    bool wichtig;
    QString anlass;
    QString wagen;
    QList<QString> *tf;
    QList<QString> *zf;
    QList<QString> *zub;
    QList<QString> *service;
    QTime zeitTf;
    QTime zeitZ;
    QString bemerkungen;
    ManagerReservierungen *reservierungen;
    bool benoetigeTf;
    bool benoetigeZf;
    bool benoetigeZub;
    bool benoetigeService;

    QJsonArray qListToJsonArray(QList<QString> *list);
    QList<QString> *jsonArrayToQList(QJsonArray array);


public slots:
    // Wichtiges
    void update();

    QJsonObject toJson();

    void fromJson(QJsonObject o);

    // GETTER
    QDate getDatum();
    int getArt();
    bool getWichtig();
    QString getAnlass();
    QString getWagenreihung();
    QTime getTimeTf();
    QTime getTimeZ();

    QList<QString>* getTf();
    QList<QString>* getZf();
    QList<QString>* getZub();
    QList<QString>* getService();
    QString getBemerkungen();
    ManagerReservierungen* getManager();

    QListWidgetItem *getItem();

    bool getBenoetigeTf() const;
    bool getBenoetigeZf() const;
    bool getBenoetigeZub() const;
    bool getBenoetigeService() const;


    // SETTER
    void setDatum(QDate datum);
    void setArt(int art);
    void setWichtig(bool wichtig);
    void setAnlass(QString anlass);
    void setWagenreihung(QString wagenreihung);
    void setTimeTf(QTime zeit);
    void setTimeZ(QTime zeit);

    void setTf(QList<QString> *text);
    void setZf(QList<QString> *text);
    void setZub(QList<QString> *text);
    void setService(QList<QString> *text);
    void setBemerkungen(QString text);
    void setHidden(bool hide); // Durchreichen zu item

    void setBenoetigeTf(bool value);
    void setBenoetigeZf(bool value);
    void setBenoetigeZub(bool value);
    void setBenoetigeService(bool value);

};

#endif // FAHRTAG_H
