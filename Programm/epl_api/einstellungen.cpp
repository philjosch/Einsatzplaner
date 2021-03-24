#include "einstellungen.h"

#include <QDir>

QSettings Einstellungen::SETTINGS;

bool Einstellungen::getAutoSearchUpdate()
{
    return SETTINGS.value("general/autosearchupdate", true).toBool();
}
void Einstellungen::setAutoSearchUpdate(bool value)
{
    SETTINGS.setValue("general/autosearchupdate", value);
}

bool Einstellungen::getUseAutoUpload()
{
    return SETTINGS.value("online/useautoupload", true).toBool();
}
void Einstellungen::setUseAutoUpload(bool value)
{
    SETTINGS.setValue("online/useautoupload", value);
}

int Einstellungen::getAutoSave()
{
    return SETTINGS.value("io/autoSave", 4).toInt();
}
void Einstellungen::setAutoSave(int value)
{
    SETTINGS.setValue("io/autoSave", value);
}

Einstellungen::ReihenfolgeSortierung Einstellungen::getReihenfolgeVorNach()
{
    if (SETTINGS.value("general/nameOrder", true).toBool())
        return VornameNachname;
    else
        return NachnameVorname;
}
void Einstellungen::setReihenfolgeVorNach(ReihenfolgeSortierung value)
{
    switch (value) {
    case VornameNachname:
        SETTINGS.setValue("general/nameOrder", true);
        return;
    case NachnameVorname:
        SETTINGS.setValue("general/nameOrder", false);
        return;
    }
}

QString Einstellungen::getLastPath()
{
    if (SETTINGS.contains("io/lastpath")) {
        return SETTINGS.value("io/lastpath").toString();
    } else {
        return QDir::homePath();
    }
}
void Einstellungen::setLastPath(QString value)
{
    SETTINGS.setValue("io/lastpath", value);
}

QStringList Einstellungen::getLastUsed()
{
    if (SETTINGS.contains("io/lastused"))
        return SETTINGS.value("io/lastused").toStringList();
    else
        return QStringList();
}
void Einstellungen::setLastUsed(QStringList value)
{
    SETTINGS.setValue("io/lastused", value);
}

QString Einstellungen::getBenutzername()
{
    QString name = qEnvironmentVariable("USER");
    if (name.isEmpty())
            name = qEnvironmentVariable("USERNAME");
    return name;
}
