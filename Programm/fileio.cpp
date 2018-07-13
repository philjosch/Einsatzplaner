#include "Fileio.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QSettings>

QString FileIO::currentPath = QDir::homePath();
QStringList FileIO::lastUsed = QStringList();

void FileIO::saveSettings()
{
    QSettings settings;
    settings.setValue("io/lastpath", currentPath);
    settings.setValue("io/lastused", lastUsed);
}

void FileIO::loadSettings()
{
    QSettings settings;
    if (settings.contains("io/lastpath")) {
        currentPath = settings.value("io/lastpath").toString();
    } else {
        currentPath = QDir::homePath();
    }
    if (settings.contains("io/lastused"))
        lastUsed = settings.value("io/lastused").toStringList();
    else
        lastUsed = QStringList();
}

QString FileIO::getFilePathOpen(QWidget *parent, QString filter)
{
    QString path = QFileDialog::getOpenFileName(parent, parent->tr("Datei öffnen ..."), currentPath, filter);
    if (path != "") {
        QFileInfo info(path);
        currentPath = info.absolutePath();
        saveSettings();
    }
    return path;
}

QString FileIO::getFilePathSave(QWidget *parent, QString filename, QString filter)
{
    QString path = QFileDialog::getSaveFileName(parent, parent->tr("Datei speichern ..."), currentPath+"/"+filename, filter);
    if (path != "") {
        QFileInfo info(path);
        currentPath = info.absolutePath();
        saveSettings();
    }
    return path;
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
    datei.close();
    insert(filepath);
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
        insert(filepath);
        return true;
    }
}

QStringList FileIO::getLastUsed()
{
    loadSettings();
    return lastUsed;
}

void FileIO::clearLastUsed()
{
    lastUsed = QStringList();
    saveSettings();
}

void FileIO::insert(QString filepath)
{
    if (filepath.endsWith(".ako", Qt::CaseInsensitive)) {
        if (lastUsed.contains(filepath))
            lastUsed.removeOne(filepath);
        else if (lastUsed.length() >= 5)
            lastUsed.removeLast();
        lastUsed.insert(0, filepath);
        saveSettings();
    }
}
