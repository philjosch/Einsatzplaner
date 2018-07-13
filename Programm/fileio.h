#ifndef FILEIO_H
#define FILEIO_H

#include <QWidget>
#include <QFileDialog>

class FileIO
{
public:
    static void saveSettings();
    static void loadSettings();

    static QString getFilePathOpen(QWidget *parent, QString filter);
    static QString getFilePathSave(QWidget *parent, QString filename, QString filter);

    static QJsonObject getJsonFromFile(QString filepath);
    static bool saveJsonToFile(QString filepath, QJsonObject object);

    static QStringList getLastUsed();
    static void clearLastUsed();

private:
    static QString currentPath;
    static QStringList lastUsed;

    static void insert(QString filepath);
};

#endif // FILEIO_H
