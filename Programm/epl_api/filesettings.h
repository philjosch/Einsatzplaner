#ifndef FILESETTINGS_H
#define FILESETTINGS_H

#include "networking.h"
#include "auswahl.h"

#include <QJsonObject>

class FileSettings : public QObject
{
    Q_OBJECT

public:
    explicit FileSettings();
    explicit FileSettings(QJsonObject json);

    QJsonObject toJson() const;

    bool getEnabled() const;
    void setEnabled(bool value);

    bool getAutom() const;
    void setAutom(bool value);

    Networking::Server getServer() const;
    void setServer(const Networking::Server &value);

    Auswahl getAuswahl() const;
    void setAuswahl(const Auswahl &value);

signals:
    void changed();

protected:
    bool enabled;
    bool autom;
    Networking::Server server;
    Auswahl auswahl;

};

#endif // FILESETTINGS_H
