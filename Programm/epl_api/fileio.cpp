#include "einstellungen.h"
#include "fileio.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QByteArray>
#include <QMessageBox>
#include <QFileDialog>

QString FileIO::currentPath = Einstellungen::getLastPath();
QStringList FileIO::lastUsed = Einstellungen::getLastUsed();

void FileIO::saveSettings()
{
    Einstellungen::setLastPath(currentPath);
    Einstellungen::setLastUsed(lastUsed);
}

QString FileIO::getFilePathOpen(QWidget *parent, QString filter)
{
    QString path = QFileDialog::getOpenFileName(parent, QObject::tr("Datei öffnen ..."), currentPath, filter);
    if (path == "") return "";
    QFileInfo info(path);
    currentPath = info.absolutePath();
    saveSettings();
    return path;
}

QString FileIO::getFilePathSave(QWidget *parent, QString filename, QString filter)
{
    QString path = QFileDialog::getSaveFileName(parent, QObject::tr("Datei speichern ..."), currentPath+"/"+filename, filter);
    if (path == "") return "";

    if (!Schreibschutz::pruefen(path).isEmpty()) {
        QMessageBox::warning(parent, QObject::tr("Datei geöffnet"), QObject::tr("Unter der angegebenen Adresse befindet sich eine Datei, die aktuell geöffnet und bearbeitet wird. Ein Speichern ist somit nicht möglich!"));
        return "";
    }

    QFileInfo info(path);
    currentPath = info.absolutePath();
    saveSettings();
    return path;
}

QJsonObject FileIO::getJsonFromFile(QString filepath)
{
    QByteArray data = readFromFile(filepath);
    if (data.isEmpty())
        return QJsonObject();
    QJsonDocument *json = new QJsonDocument(QJsonDocument::fromJson(data));
    History::insert(filepath);
    return json->object();
}

bool FileIO::saveJsonToFile(QString filepath, QJsonObject object)
{
    QJsonDocument saveDoc = QJsonDocument(object);
    return saveToFile(filepath, saveDoc.toJson());
}

QStringList FileIO::History::get()
{
    return Einstellungen::getLastUsed();
}

void FileIO::History::clear()
{
    lastUsed = QStringList();
    saveSettings();
}

bool FileIO::saveToFile(QString path, QString content)
{
    QFile datei(path);
    if (!datei.open(QIODevice::WriteOnly)) {
        return false;
    }
    datei.write(content.toUtf8());
    datei.close();
    return true;
}

QByteArray FileIO::readFromFile(QString path)
{
    QFile datei(path);
    if(!datei.open(QIODevice::ReadOnly)) {
        return QByteArray();
    }
    QByteArray data = datei.readAll();
    datei.close();
    return data;
}

bool FileIO::removeFile(QString path)
{
    return QFile::remove(path);
}

bool FileIO::Schreibschutz::setzen(QString dateiPfad)
{
    if (!Schreibschutz::pruefen(dateiPfad+".lock").isEmpty())
        return false;
    QString text = "";
    text += QDateTime::currentDateTime().toString(QObject::tr("dd.MM.yyyy hh:mm:ss"));
    text += ";" + Einstellungen::getBenutzername();
    return saveToFile(dateiPfad+".lock", text);
}

QStringList FileIO::Schreibschutz::pruefen(QString dateipfad)
{
    QString s = QString(readFromFile(dateipfad+".lock"));
    if (s == "") return QStringList();
    return s.split(";");
}

bool FileIO::Schreibschutz::freigeben(QString dateipfad)
{
    return removeFile(dateipfad+".lock");
}

void FileIO::History::insert(QString filepath)
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
