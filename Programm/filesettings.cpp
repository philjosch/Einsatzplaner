#include "filesettings.h"

FileSettings::FileSettings()
{
    server = Networking::Server{"", "", ""};
    auswahl = AActivity::Auswahl();

}

void FileSettings::fromJson(QJsonObject obj)
{
    QJsonObject online = obj.value("online").toObject();
    enabled = online.value("enabled").toBool();
    autom = online.value("auto").toBool(true);
    server = Networking::Server::fromJson(online);
    auswahl = AActivity::Auswahl::fromJson(online);
}

QJsonObject FileSettings::toJson()
{
    QJsonObject o;
    o.insert("enabled", enabled);
    o.insert("auto", autom);
    server.insertJson(&o);
    auswahl.insertJson(&o);
    QJsonObject all;
    all.insert("online", o);
    return all;
}

bool FileSettings::getEnabled() const
{
    return enabled;
}
void FileSettings::setEnabled(bool value)
{
    enabled = value;
}

bool FileSettings::getAutom() const
{
    return autom;
}
void FileSettings::setAutom(bool value)
{
    autom = value;
}

AActivity::Auswahl FileSettings::getAuswahl() const
{
    return auswahl;
}

void FileSettings::setAuswahl(const AActivity::Auswahl &value)
{
    auswahl = value;
}

Networking::Server FileSettings::getServer() const
{
    return server;
}

void FileSettings::setServer(const Networking::Server &value)
{
    server = value;
}
