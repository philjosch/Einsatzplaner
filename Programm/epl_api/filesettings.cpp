#include "filesettings.h"

#include <crypto.h>

using namespace Cryptography;

FileSettings::FileSettings() : QObject()
{
    enabled = false;
    autom = true;
    server = Networking::Server{"", "", ""};
    auswahl = Auswahl(Auswahl::AbJetzt, Auswahl::BisAlle, true);
}

FileSettings::FileSettings(QJsonObject json) : QObject()
{
    QJsonObject online = json.value("online").toObject();
    enabled = online.value("enabled").toBool();
    autom = online.value("auto").toBool(true);
    server = Networking::Server::fromJson(online);
    auswahl = Auswahl(online);

}

QJsonObject FileSettings::toJson() const
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
    emit changed();
}

bool FileSettings::getAutom() const
{
    return autom;
}
void FileSettings::setAutom(bool value)
{
    autom = value;
    emit changed();
}

Auswahl FileSettings::getAuswahl() const
{
    return auswahl;
}
void FileSettings::setAuswahl(const Auswahl &value)
{
    auswahl = value;
    emit changed();
}

Networking::Server FileSettings::getServer() const
{
    return server;
}
void FileSettings::setServer(const Networking::Server &value)
{
    server = value;
    emit changed();
}
