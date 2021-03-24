#ifndef EPLFILE_H
#define EPLFILE_H

#include "manager.h"
#include "managerpersonal.h"

#include <QObject>

class EplFile : public QObject
{
    Q_OBJECT

public:
    struct FensterPosition { int x, y, width, height; };

    EplFile(QString dateiPfad = "");

    QString getPfad() const;

    bool istSchreibgeschuetzt() const;
    QStringList getInfoSchreibschutz() const;

    bool istPasswortGeschuetzt();

    bool istGespeichert() const;
    void veraendern();


    Manager *getManager() const;
    ManagerPersonal *getPersonal() const;
    FileSettings *getDateiEigenschaften() const;

    QDate getAnzeigeDatum() const;
    void setAnzeigeDatum(const QDate &value);

    FensterPosition getPositionKalender();
    void setPositionKalender(const FensterPosition &value);

    FensterPosition getPositionPersonal();
    void setPositionPersonal(const FensterPosition &value);


public slots:
    void speichern();
    void speichernUnter(QString path);
    void speichernPersonal(QString pfad);
    void autoSave();

    void open(QString passw = "");
    void close();

signals:
    void changed();

protected:
    QString pfad;
    bool schreibgeschuetzt;
    bool gespeichert;

    QJsonObject geladen;

    Manager *manager;
    ManagerPersonal *personal;
    FileSettings *dateiEigenschaften;

    QDate currentDate;
    FensterPosition positionKalender;
    FensterPosition positionPersonal;

    QJsonObject generiereJson() const;
    QJsonObject generiereJsonPersonal() const;

    bool schreibeJsonInDatei(QString pfad, QJsonObject obj) const;

    QString komprimiere(QJsonObject obj) const;
    QJsonObject dekomprimiere(QString komprimiert) const;
};

#endif // EPLFILE_H
