#include "networking.h"

#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>

QString Networking::ladeDatenVonURL(QString url)
{
    // create custom temporary event loop on stack
    QEventLoop eventLoop;

    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    // the HTTP request
    QNetworkRequest req( url );
    QNetworkReply *reply = mgr.get(req);
    eventLoop.exec(); // blocks stack until "finished()" has been called

    QString s = "";
    if (reply->error() == QNetworkReply::NoError) {
        //success
        QString s = QString(reply->readAll());
    }
    delete reply;
    return s;
}

bool Networking::ladeDateiHoch(QString url, QFile *datei, QString name)
{
    QEventLoop eventLoop;

    QNetworkAccessManager am;
    QObject::connect(&am, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    QNetworkRequest request(url);

    QByteArray data;
    data.append("--margin\r\n");
    data.append("Content-Disposition: form-data; name='id'\r\n\r\n"+name+"\r\n");
    data.append("--margin\r\n");
    data.append("Content-Disposition: form-data; name='uploaded'; filename='"+name+".pdf'\r\n");
    data.append("Content-Type: application/pdf\r\n\r\n");
    data.append(datei->readAll());
    data.append("\r\n");
    data.append("--margin--\r\n"); //closing boundary according to rfc 1867
    request.setRawHeader(QString("Content-Type").toUtf8(), QString("multipart/form-data; boundary=margin").toUtf8());
    request.setRawHeader(QString("Content-Length").toUtf8(), QString::number(data.length()).toUtf8());

    QNetworkReply *reply = am.post(request,data);
    eventLoop.exec();

    QString s = "";
    if (reply->error() == QNetworkReply::NoError) {
        s = QString(reply->readAll());
    }
    delete reply;
    return (s == "OK");
}
