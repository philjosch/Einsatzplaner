#ifndef FILESETTINGS_H
#define FILESETTINGS_H

#include "aactivity.h"
#include "networking.h"

#include <QJsonObject>

class FileSettings : public QObject
{
    Q_OBJECT

public:
    FileSettings();
    FileSettings(QJsonObject json);

    QJsonObject toJson();

    bool getEnabled() const;
    void setEnabled(bool value);

    bool getAutom() const;
    void setAutom(bool value);

    Networking::Server getServer() const;
    void setServer(const Networking::Server &value);

    AActivity::Auswahl getAuswahl() const;
    void setAuswahl(const AActivity::Auswahl &value);

    /**
     * @brief Gibt das gehashte Passwort zurueck, mit dem der Zugriff auf die Datei gesichert wird
     * @return Das gehashte Passwort
     */
    QString getPasswort() const;
    /**
     * @brief Setzen des Passworts, das den Zugriff auf die Datei schuetzt
     * @param value: Das ungehashte Passwort
     */
    void setPasswort(const QString &value);

signals:
    void changed();

protected:
    bool enabled = false;
    bool autom = true;
    Networking::Server server;
    AActivity::Auswahl auswahl;

    QString passwort = "";
};

#endif // FILESETTINGS_H
