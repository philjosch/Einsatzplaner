#include "networking.h"

#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>

QString Networking::ladeDatenVonURL(QString url)
{
    if (!url.startsWith("http")) {
        url = "https://"+url;
    }
    QNetworkRequest req( url );
    return get(req);
}

bool Networking::ladeDateiHoch(Networking::Server server, QFile *datei)
{
    QString url = server.getServerKomplett();
    if (!url.startsWith("http")) {
        url = "https://"+url;
    }

    QNetworkRequest request(url);

    QByteArray data;
    data.append("--margin\r\n");
    data.append("Content-Disposition: form-data; name='id'\r\n\r\n"+server.id+"\r\n");
    data.append("--margin\r\n");
    data.append("Content-Disposition: form-data; name='uploaded'; filename='"+server.id+".pdf'\r\n");
    data.append("Content-Type: application/pdf\r\n\r\n");
    data.append(datei->readAll());
    data.append("\r\n");
    data.append("--margin--\r\n"); //closing boundary according to rfc 1867
    request.setRawHeader(QString("Content-Type").toUtf8(), QString("multipart/form-data; boundary=margin").toUtf8());
    request.setRawHeader(QString("Content-Length").toUtf8(), QString::number(data.length()).toUtf8());

    QString s = post(request, data);
    return (s == "OK");
}

bool Networking::testServerVerbindung(Networking::Server server)
{
    return (Networking::ladeDatenVonURL(server.getServerKomplettFuerTest()) == "OK");
}

QString Networking::post(QNetworkRequest anfrage, QByteArray daten)
{
    QEventLoop eventLoop;

    QNetworkAccessManager am;
    QObject::connect(&am, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    QNetworkReply *reply = am.post(anfrage,daten);
    eventLoop.exec();

    QString s = "";
    if (reply->error() == QNetworkReply::NoError) {
        s = QString(reply->readAll());
    }
    delete reply;
    return s;
}

QString Networking::get(QNetworkRequest anfrage)
{
    QEventLoop eventLoop;

    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    // the HTTP request
    QNetworkReply *reply = mgr.get(anfrage);
    eventLoop.exec(); // blocks stack until "finished()" has been called

    QString s = "";
    if (reply->error() == QNetworkReply::NoError) {
        //success
        s = QString(reply->readAll());
    }
    delete reply;
    return s;
}
