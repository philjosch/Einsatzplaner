#include "Fileio.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

QString FileIO::currentPath = "";

QString FileIO::getFilePathOpen(QWidget *parent)
{
    QUrl path = QFileDialog::getOpenFileUrl(parent, parent->tr("Datei öffnen ..."), currentPath, parent->tr("AkO-Dateien (*.ako)"));
    currentPath = path.path();
    currentPath = currentPath.remove(path.fileName());
    return path.path();
}

QString FileIO::getFilePathSave(QWidget *parent)
{
    QUrl path = QFileDialog::getSaveFileUrl(parent, parent->tr("Datei speichern ..."), currentPath, parent->tr("AkO-Dateien (*.ako)"));
    currentPath = path.path();
    currentPath = currentPath.remove(path.fileName());
    return path.path();
}

QJsonObject FileIO::getJsonFromFile(QString filepath)
{
    QFile datei(filepath);
    if(!datei.open(QIODevice::ReadOnly)) {
        return QJsonObject();
    }
    QByteArray data = datei.readAll();
    QJsonDocument tmp = QJsonDocument::fromJson(data);
    QJsonDocument *json = new QJsonDocument(tmp);

    return json->object();
}

bool FileIO::saveJsonToFile(QString filepath, QJsonObject object)
{
    QJsonDocument saveDoc = QJsonDocument(object);
    QFile datei(filepath);
    if (!datei.open(QIODevice::WriteOnly)) {
        return false;
    } else {
        datei.write(saveDoc.toJson());
        datei.close();
        return true;
    }
}
