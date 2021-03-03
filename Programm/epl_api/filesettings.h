#ifndef FILESETTINGS_H
#define FILESETTINGS_H

#include "basics.h"
#include "networking.h"

#include <QJsonObject>

class FileSettings : public QObject
{
    Q_OBJECT

public:
    FileSettings();
    FileSettings(QJsonObject json, QString pwd);

    QJsonObject toJson();

    bool getEnabled() const;
    void setEnabled(bool value);

    bool getAutom() const;
    void setAutom(bool value);

    Networking::Server getServer() const;
    void setServer(const Networking::Server &value);

    Auswahl getAuswahl() const;
    void setAuswahl(const Auswahl &value);

    /**
     * @brief Gibt das gehashte Passwort zurueck, mit dem der Zugriff auf die Datei gesichert wird
     * @return Das gehashte Passwort
     */
    QString getPasswort() const;
    /**
     * @brief Setzen des Passworts, das den Zugriff auf die Datei schuetzt
     * @param value: Das ungehashte Passwort
     */
    bool setPasswort(const QString &neu, const QString &alt = "");

signals:
    void changed();

protected:
    bool enabled = false;
    bool autom = true;
    Networking::Server server;
    Auswahl auswahl;

    QString passwort = "";
};

#endif // FILESETTINGS_H
