#include "Fileio.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QSettings>

QString FileIO::currentPath = QDir::homePath();

void FileIO::saveSettings()
{
    QSettings settings;
    settings.setValue("io/lastpath", currentPath);
}

void FileIO::loadSettings()
{
    QSettings settings;
    if (settings.contains("io/lastpath")) {
        currentPath = settings.value("io/lastpath").toString();
    } else {
        currentPath = QDir::homePath();
    }
}

QString FileIO::getFilePathOpen(QWidget *parent, QString filter)
{
    QUrl path = QFileDialog::getOpenFileUrl(parent, parent->tr("Datei öffnen ..."), currentPath, filter);
    if (path.path() != "") {
        currentPath = path.path();
        currentPath = currentPath.remove(path.fileName());
    }
    qDebug() << path.path();
    qDebug() << currentPath;
    saveSettings();
    return path.path();
}

QString FileIO::getFilePathSave(QWidget *parent, QString filename, QString filter)
{
    qDebug() << currentPath+"/"+filename;
    QUrl path = QFileDialog::getSaveFileUrl(parent, parent->tr("Datei speichern ..."), currentPath+"/"+filename, filter);
    if (path.path() != "") {
        currentPath = path.path();
        currentPath = currentPath.remove(path.fileName());
    }
    saveSettings();
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
