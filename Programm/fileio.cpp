#include "fileio.h"

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
    QString path = QFileDialog::getOpenFileName(parent, QObject::tr("Datei öffnen ..."), currentPath, filter);
    if (path != "") {
        QFileInfo info(path);
        currentPath = info.absolutePath();
        saveSettings();
    }
    return path;
}

QString FileIO::getFilePathSave(QWidget *parent, QString filename, QString filter)
{
    QString path = QFileDialog::getSaveFileName(parent, QObject::tr("Datei speichern ..."), currentPath+"/"+filename, filter);
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

bool FileIO::saveJsonToFile(QString filepath, QJsonObject object, bool showInMenu)
{
    QJsonDocument saveDoc = QJsonDocument(object);
    if (saveToFile(filepath, saveDoc.toJson())) {
        if (showInMenu) insert(filepath);
        return true;
    }
    return false;
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

bool FileIO::saveToFile(QString path, QString content)
{
    QFile datei(path);
    if (datei.open(QIODevice::WriteOnly)) {
        datei.write(content.toUtf8());
        datei.close();
        return true;
    }
    return false;
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
