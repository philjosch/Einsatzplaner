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
    static bool saveJsonToFile(QString filepath, QJsonObject object, bool showInMenu=true);

    static QStringList getLastUsed();
    static void clearLastUsed();

    static bool saveToFile(QString path, QString content);

private:
    static QString currentPath;
    static QStringList lastUsed;

    static void insert(QString filepath);
};

#endif // FILEIO_H
