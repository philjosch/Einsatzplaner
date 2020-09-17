#ifndef FILEIO_H
#define FILEIO_H

#include <QWidget>
#include <QFileDialog>

class FileIO
{
public:
    static void saveSettings();

    static QString getFilePathOpen(QWidget *parent, QString filter);
    static QString getFilePathSave(QWidget *parent, QString filename, QString filter);

    static QJsonObject getJsonFromFile(QString filepath);
    static bool saveJsonToFile(QString filepath, QJsonObject object, bool showInMenu=true);

    static QStringList getLastUsed();
    static void clearLastUsed();

    static bool saveToFile(QString path, QString content);
    class Schreibschutz {
    public:
        static bool setzen(QString dateiPfad);
        static QStringList pruefen(QString dateipfad);
        static bool freigeben(QString dateipfad);
    };

private:
    static QString currentPath;
    static QStringList lastUsed;

    static void insert(QString filepath);
    static QString getBenutzername();
};

#endif // FILEIO_H
