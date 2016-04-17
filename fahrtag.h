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
    QDate Datum;
    int Art;
    bool Wichtig;
    QString Anlass;
    QString wagen;
    QList<QString> *Tf;
    QList<QString> *Zf;
    QList<QString> *Zub;
    QList<QString> *Service;
    QTime ZeitTf;
    QTime ZeitZ;
    QString Bemerkungen;
    ManagerReservierungen *reservierungen;
    bool benoetigeTf;
    bool benoetigeZf;
    bool benoetigeZub;
    bool benoetigeService;

public slots:
    // Wichtiges
    void update();

    QJsonObject toJson();
    QJsonArray qListToJsonArray(QList<QString> *list);
    QList<QString> *jsonArrayToQList(QJsonArray array);

    void fromJson(QJsonObject o);

    void verteileSitzplaetze();

    // GETTER
    QDate getDatum();
    int getArt();
    bool getWichtig();
    QString getAnlass();
    QString getWagenreihung();

    QList<Wagen*> *getErsteKlasseWagen();
    QList<Wagen*> *getAndereKlasseWagen();

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
