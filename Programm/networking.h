#ifndef NETWORKING_H
#define NETWORKING_H

#include <QFile>
#include <QJsonObject>

class Networking
{
public:
    struct Server {
        QString server;
        QString path;
        QString id;

        QString getServerKomplett() {
            return server + "/" + path;
        }
        QString getServerKomplettFuerTest() {
            return server + "/" + path + "?id=" + id;
        }
        void insertJson(QJsonObject *o) {
            o->insert("server", server);
            o->insert("path", path);
            o->insert("id", id);
        }
        static Server fromJson(QJsonObject o) {
            return Server {o.value("server").toString(),
                        o.value("path").toString(),
                        o.value("id").toString()};
        }
    };

    static QString ladeDatenVonURL(QString url);
    static bool ladeDateiHoch(Networking::Server server, QFile *datei);

    static bool testServerVerbindung(Networking::Server server);
};

#endif // NETWORKING_H
