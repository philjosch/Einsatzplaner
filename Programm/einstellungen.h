#ifndef EINSTELLUNGEN_H
#define EINSTELLUNGEN_H

#include <QSettings>



class Einstellungen
{
public:
    static bool getAutoSearchUpdate();
    static void setAutoSearchUpdate(bool value);

    static bool getUseAutoUpload();
    static void setUseAutoUpload(bool value);

    static int getAutoSave();
    static void setAutoSave(int value);

    static bool getReihenfolgeVorNach();
    static void setReihenfolgeVorNach(bool value);

    static QString getLastPath();
    static void setLastPath(QString value);

    static QStringList getLastUsed();
    static void setLastUsed(QStringList value);

private:
    static QSettings settings;
};

#endif // EINSTELLUNGEN_H
