#ifndef FILESETTINGS_H
#define FILESETTINGS_H

#include "aactivity.h"
#include "networking.h"

#include <QJsonObject>

class FileSettings
{
public:
    FileSettings();

    void fromJson(QJsonObject obj);
    QJsonObject toJson();

    bool getEnabled() const;
    void setEnabled(bool value);

    bool getAutom() const;
    void setAutom(bool value);

    Networking::Server getServer() const;
    void setServer(const Networking::Server &value);

    AActivity::Auswahl getAuswahl() const;
    void setAuswahl(const AActivity::Auswahl &value);

protected:
    bool enabled = false;
    bool autom = true;
    Networking::Server server;
    AActivity::Auswahl auswahl;
};

#endif // FILESETTINGS_H
