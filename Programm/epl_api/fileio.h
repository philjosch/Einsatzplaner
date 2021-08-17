#ifndef FILEIO_H
#define FILEIO_H

#include <QWidget>

class FileIO
{
public:
    enum DateiTyp {EPL,
                   EPLAutoSave,
                   PDF,
                   CSV};

    static QString getFilePathOpen(QWidget *parent, DateiTyp typ);
    static QString getFilePathSave(QWidget *parent, QString filename, DateiTyp typ);

    static QJsonObject getJsonFromFile(QString filepath);
    static bool saveJsonToFile(QString filepath, QJsonObject object);

    class History{
    public:
        static QStringList get();
        static void clear();
        static void insert(QString filepath);
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

    static QString getSuffixVonTyp(DateiTyp typ);

private:
    static QString getFilterVonTyp(DateiTyp typ);

};

#endif // FILEIO_H
