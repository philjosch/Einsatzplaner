#ifndef NETWORKING_H
#define NETWORKING_H

#include <QFile>

class Networking
{
public:
    static QString ladeDatenVonURL(QString url);
    static bool ladeDateiHoch(QString url, QFile *datei, QString name);
};

#endif // NETWORKING_H