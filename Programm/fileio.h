#ifndef FILEIO_H
#define FILEIO_H

#include <QWidget>
#include <QFileDialog>

class FileIO
{
public:

    static QString getFilePathOpen(QWidget *parent);
    static QString getFilePathSave(QWidget *parent);

    static QJsonObject getJsonFromFile(QString filepath);
    static bool saveJsonToFile(QString filepath, QJsonObject object);


private:
    static QString currentPath;
};

#endif // FILEIO_H
