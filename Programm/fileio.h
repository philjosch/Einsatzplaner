#ifndef FILEIO_H
#define FILEIO_H

#include <QWidget>
#include <QFileDialog>

class FileIO
{
public:
    static QString getFilePathOpen(QWidget *parent, QString filter);
    static QString getFilePathSave(QWidget *parent, QString filename, QString filter);

    static QJsonObject getJsonFromFile(QString filepath);
    static bool saveJsonToFile(QString filepath, QJsonObject object, bool showInMenu=true);

    class History{
    public:
        static QStringList get();
        static void clear();
    };

    static bool saveToFile(QString path, QString content);

private:
    static QString currentPath;
    static QStringList lastUsed;

    static void insert(QString filepath);
    static void saveSettings();

};

#endif // FILEIO_H
