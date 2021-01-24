#ifndef EPLEXCEPTION_H
#define EPLEXCEPTION_H

#include "version.h"

#include <QString>
#include <QObject>



class EplException
{
public:
    EplException();
    EplException(QString msg);
    virtual QString getError() const;

protected:
    QString message;
};

class FileException : public EplException {
public: FileException(QString msg) : EplException(msg) {}
    FileException() : EplException("Beim Laden der Datei ist ein Fehler aufgetreten.") {}
};
class FileToNewException : public FileException {
public: FileToNewException(Version version) : FileException(QObject::tr("Die Datei kann nicht geladen werden, da sie mindestens Version %1 benötigt.").arg(version.toStringShort())) {}
};
class FileToOldException : public FileException {
public: FileToOldException(Version version) : FileException(QObject::tr("Die Datei kann nicht geladen werden, da sie Version %1 benötigt.").arg(version.toStringShort())) {}
};
class FileVersionNotSupportedException : public FileException {
public: FileVersionNotSupportedException(QString msg) : FileException(msg) {}
};
class FileWriteProtectedException : public FileException {
public: FileWriteProtectedException() : FileException("Die Datei konnte nicht gespeichert werden, da sie schreibgeschützt geöffnet wurde.") {}
};
class FilePathInvalidException : public FileException {
public: FilePathInvalidException() : FileException("Der angegebene Dateipfad ist ungültig.") {}
};
class FileWriteException : public FileException {
public: FileWriteException() : FileException("Die Datei konnte nicht unter dem angegebenen Dateipfad gespeichert werden.") {}
};



class AActivityException : public EplException {
public: AActivityException(QString msg) : EplException(msg) {}
    AActivityException() : EplException("Während der Verarbeitung der Eingabe ist ein Fehler unterlaufen.\nPrüfen Sie Ihre Eingaben und versuchen es erneut!") {}
};
class FalscheQualifikationException : public AActivityException {
public: FalscheQualifikationException(QString name) : AActivityException(QObject::tr("Die Aufgabe kann/darf nicht von %1 übernommen werden, da eine betriebliche Ausbildung und gültige Tauglichkeitsuntersuchung benötigt wird.").arg(name)) {}
};
class PersonNichtGefundenException : public AActivityException {
public: PersonNichtGefundenException(QString name) : AActivityException(QObject::tr("%1 konnte nicht gefunden werden!\nBitte überprüfen Sie Ihre Eingabe und legen die Person gegebenenfalls neu an.").arg(name)) {}
};



#endif // EPLEXCEPTION_H
