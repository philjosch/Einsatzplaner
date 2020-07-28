#include "einstellungen.h"

#include <QDir>

QSettings Einstellungen::settings;

bool Einstellungen::getAutoSearchUpdate()
{
    return settings.value("general/autosearchupdate", true).toBool();
}

void Einstellungen::setAutoSearchUpdate(bool value)
{
    settings.setValue("general/autosearchupdate", value);
}

bool Einstellungen::getUseAutoUpload()
{
    return settings.value("online/useautoupload", true).toBool();
}

void Einstellungen::setUseAutoUpload(bool value)
{
    settings.setValue("online/useautoupload", value);
}

int Einstellungen::getAutoSave()
{
    return settings.value("io/autoSave", 4).toInt();
}

void Einstellungen::setAutoSave(int value)
{
    settings.setValue("io/autoSave", value);
}

bool Einstellungen::getReihenfolgeVorNach()
{
    return settings.value("general/nameOrder", true).toBool();
}

void Einstellungen::setReihenfolgeVorNach(bool value)
{
    settings.setValue("general/nameOrder", value);
}

QString Einstellungen::getLastPath()
{
    if (settings.contains("io/lastpath")) {
        return settings.value("io/lastpath").toString();
    } else {
        return QDir::homePath();
    }
}

void Einstellungen::setLastPath(QString value)
{
    settings.setValue("io/lastpath", value);
}

QStringList Einstellungen::getLastUsed()
{
    if (settings.contains("io/lastused"))
        return settings.value("io/lastused").toStringList();
    else
        return QStringList();
}

void Einstellungen::setLastUsed(QStringList value)
{
    settings.setValue("io/lastused", value);
}
