#ifndef EINSTELLUNGEN_H
#define EINSTELLUNGEN_H

#include <QSettings>

class Einstellungen
{
public:
    enum ReihenfolgeSortierung {
        VornameNachname = 1,
        NachnameVorname = 2,
    };

    static bool getAutoSearchUpdate();
    static void setAutoSearchUpdate(bool value);

    static bool getUseAutoUpload();
    static void setUseAutoUpload(bool value);

    static int getAutoSave();
    static void setAutoSave(int value);

    static ReihenfolgeSortierung getReihenfolgeVorNach();
    static void setReihenfolgeVorNach(ReihenfolgeSortierung value);

    static QString getLastPath();
    static void setLastPath(QString value);

    static QStringList getLastUsed();
    static void setLastUsed(QStringList value);

    static QString getBenutzername();

private:
    static QSettings SETTINGS;
};

#endif // EINSTELLUNGEN_H
