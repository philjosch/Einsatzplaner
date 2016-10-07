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


private:
    static QString currentPath;
};

#endif // FILEIO_H
