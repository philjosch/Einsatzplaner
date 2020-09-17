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
    static QByteArray readFromFile(QString path);
    static bool removeFile(QString path);


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
        static void saveSettings();

    static QString getBenutzername();
};

#endif // FILEIO_H
