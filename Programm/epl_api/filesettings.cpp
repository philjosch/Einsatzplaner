#include "filesettings.h"

#include <crypto.h>

using namespace Cryptography;

FileSettings::FileSettings() : QObject()
{
    enabled = false;
    autom = true;
    server = Networking::Server{"", "", ""};
    auswahl = Auswahl(Auswahl::AbJetzt, Auswahl::BisAlle, true);
    passwort = "";
}

FileSettings::FileSettings(QJsonObject json, QString pwd) : QObject()
{
    QJsonObject online = json.value("online").toObject();
    enabled = online.value("enabled").toBool();
    autom = online.value("auto").toBool(true);
    server = Networking::Server::fromJson(online);
    auswahl = Auswahl(online);

    if (pwd == "")
        passwort = "";
    else
        passwort = Crypto::hash(pwd);
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

QString FileSettings::getPasswort() const
{
    return passwort;
}
bool FileSettings::setPasswort(const QString &neu, const QString &alt)
{
    if (passwort == "") {
        if (alt != "") {
            return false;
        }
    } else {
        if (Crypto::hash(alt) != passwort) {
            return false;
        }
    }
    if (neu == "") {
        passwort = "";
    } else {
        passwort = Crypto::hash(neu);
    }
    emit changed();
    return true;
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
