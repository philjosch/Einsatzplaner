#include "managerfilesettings.h"

ManagerFileSettings::ManagerFileSettings()
{

}

void ManagerFileSettings::fromJson(QJsonObject obj)
{
    QJsonObject online = obj.value("online").toObject();
    enabled = online.value("enabled").toBool();
    autom = online.value("auto").toBool(true);
    server = online.value("server").toString();
    path = online.value("path").toString();
    id = online.value("id").toString();
    startdate = online.value("startdate").toString();
    enddate = online.value("enddate").toString();
    activities = online.value("activities").toBool(true);
}

QJsonObject ManagerFileSettings::toJson()
{
    QJsonObject o;
    o.insert("enabled", enabled);
    o.insert("auto", autom);
    o.insert("server", server);
    o.insert("path", path);
    o.insert("id", id);
    o.insert("startdate", startdate);
    o.insert("enddate", enddate);
    o.insert("activities", activities);
    QJsonObject all;
    all.insert("online", o);
    return all;
}

bool ManagerFileSettings::getEnabled() const
{
    return enabled;
}
void ManagerFileSettings::setEnabled(bool value)
{
    enabled = value;
}

bool ManagerFileSettings::getAutom() const
{
    return autom;
}
void ManagerFileSettings::setAutom(bool value)
{
    autom = value;
}

QString ManagerFileSettings::getServer() const
{
    return server;
}
void ManagerFileSettings::setServer(const QString &value)
{
    server = value;
}

QString ManagerFileSettings::getPath() const
{
    return path;
}
void ManagerFileSettings::setPath(const QString &value)
{
    path = value;
}

QString ManagerFileSettings::getFullServer() const
{
    return server + "/" + path;
}

QString ManagerFileSettings::getId() const
{
    return id;
}
void ManagerFileSettings::setId(const QString &value)
{
    id = value;
}

QString ManagerFileSettings::getStartdate() const
{
    return startdate;
}
void ManagerFileSettings::setStartdate(const QString &value)
{
    startdate = value;
}

QString ManagerFileSettings::getEnddate() const
{
    return enddate;
}
void ManagerFileSettings::setEnddate(const QString &value)
{
    enddate = value;
}

bool ManagerFileSettings::getActivities() const
{
    return activities;
}
void ManagerFileSettings::setActivities(bool value)
{
    activities = value;
}
