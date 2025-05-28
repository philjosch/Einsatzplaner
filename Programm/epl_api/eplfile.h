#ifndef EPLFILE_H
#define EPLFILE_H

#include "manager.h"
#include "managerpersonal.h"
#include "filesettings.h"

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

    /**
     * @brief Gibt das gehashte Passwort zurueck, mit dem der Zugriff auf die Datei gesichert wird
     * @return Das gehashte Passwort
     */
    QString getPasswort() const;
    /**
     * @brief Setzen des Passworts, das den Zugriff auf die Datei schuetzt
     * @param neu: Das neue ungehashte Passwort
     * @param alt: Das alte ungehashte Passwort
     */
    bool setPasswort(const QString &neu, const QString &alt = "");
    /**
     * @brief Prueft, ob ein Passwort gesetzt ist
     * @return Wahr genau dann, wenn ein Passwort gesetzt ist
     */
    bool hatPasswort() const;

public slots:
    void speichern();
    void speichernUnter(QString path);
    void speichernPersonal(QString pfad);
    void autoSave();

    void autoUpload();

    void open(QString passw = "");
    void close();

signals:
    void changed();

protected:
    QString pfad;
    bool schreibgeschuetzt;
    bool gespeichert;

    /**
     * @brief Speichert das aktuelle gehashte Passwort. Ist leer, wenn kein Passwort gesetzt ist.
     */
    QString passwort;

    QJsonObject geladen;

    Manager *manager;
    ManagerPersonal *personal;
    FileSettings *dateiEigenschaften;

    QDate currentDate;
    FensterPosition positionKalender;
    FensterPosition positionPersonal;

    /**
     * @brief Generiert ein QJsonObject, dass alle Daten enthaelt.
     * @param staticOnly: Nur die statischen Angaben sollen ausgegebn werden,
     * dynamsich sind die Aktivitaeten und zusaetzlichen Zeiten der Personen
     * @return Die statischen Daten und die dynmaischen sofern staticOnly TRUE ist
     */
    QJsonObject generiereJson(bool staticOnly) const;

    bool schreibeJsonInDatei(QString pfad, QJsonObject obj) const;

    QString komprimiere(QJsonObject obj) const;
    QJsonObject dekomprimiere(QString komprimiert) const;
};

#endif // EPLFILE_H
