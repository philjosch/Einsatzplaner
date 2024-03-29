#include "eplexception.h"
#include "eplfile.h"
#include "fileio.h"
#include "crypto.h"
#include "einstellungen.h"
#include "export.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <exception>

using namespace EplException;
using namespace Cryptography;


EplFile::EplFile(QString dateiPfad)
{
    pfad = dateiPfad;
    schreibgeschuetzt = false;
    gespeichert = true;

    geladen = QJsonObject();

    personal = new ManagerPersonal();
    manager = new Manager(personal);
    dateiEigenschaften = new FileSettings();

    currentDate = QDate::currentDate();
    currentDate = currentDate.addDays(-currentDate.day()+1);
    positionKalender = FensterPosition{};
    positionPersonal = FensterPosition{};

    connect(manager, &Manager::changed, this, &EplFile::veraendern);
    connect(personal, &ManagerPersonal::changed, this, &EplFile::veraendern);
    connect(dateiEigenschaften, &FileSettings::changed, this, &EplFile::veraendern);

    if (pfad == "")
        return;

    // Daten aus Datei laden
    geladen = FileIO::getJsonFromFile(pfad);
    if (geladen.isEmpty()) {
        throw FileException();
    }

    // Pruefen, ob Version kompatibel ist
    QJsonObject generalJSON = geladen.value("general").toObject();

    // Nicht spezifizierte Versionen sind inkompatibel
    Version version = Version(generalJSON.value("version").toString());
    if (version == Version()) {
        throw FileVersionNotSupportedException(tr("Die Datei ist nicht mit dieser Version des Einsatzplaner kompatibel."));
    }

    // Version zu alt bzw. falsch eingestellt abfangen
    if (version.getMajor() < 1) {
        throw FileVersionNotSupportedException(tr("Die Datei ist nicht mit dieser Version des Einsatzplaner kompatibel."));
    }

    // Version zu neu
    if (version > Version::getProgrammVersion()) {
        throw FileToNewException(version);
    }
}


QString EplFile::getPfad() const
{
    return pfad;
}

bool EplFile::istSchreibgeschuetzt() const
{
    return schreibgeschuetzt;
}
QStringList EplFile::getInfoSchreibschutz() const
{
    if (! schreibgeschuetzt) return QStringList();
    return FileIO::Schreibschutz::pruefen(pfad);
}

bool EplFile::istPasswortGeschuetzt()
{
    return geladen.value("general").toObject().value("encrypted").toBool();
}

bool EplFile::istGespeichert() const
{
    return gespeichert;
}
void EplFile::veraendern()
{
    gespeichert = false;
    emit changed();
}


Manager *EplFile::getManager() const
{
    return manager;
}
ManagerPersonal *EplFile::getPersonal() const
{
    return personal;
}
FileSettings *EplFile::getDateiEigenschaften() const
{
    return dateiEigenschaften;
}

QDate EplFile::getAnzeigeDatum() const
{
    return currentDate;
}
void EplFile::setAnzeigeDatum(const QDate &value)
{
    currentDate = value;
    veraendern();
}

EplFile::FensterPosition EplFile::getPositionKalender()
{
    return positionKalender;
}
void EplFile::setPositionKalender(const FensterPosition &value)
{
    positionKalender = value;
    veraendern();
}

EplFile::FensterPosition EplFile::getPositionPersonal()
{
    return positionPersonal;
}
void EplFile::setPositionPersonal(const FensterPosition &value)
{
    positionPersonal = value;
    veraendern();
}


void EplFile::speichern()
{
    if (schreibgeschuetzt) throw FileWriteProtectedException();
    if (pfad == "") throw  FilePathInvalidException();
    if (gespeichert) return;

    QJsonObject json = generiereJson(false);
    if (schreibeJsonInDatei(pfad, json)) {
        gespeichert = true;
        if (pfad != "") {
            QFile file (pfad+FileIO::getSuffixVonTyp(FileIO::DateiTyp::EPLAutoSave));
            file.remove();
        }
        FileIO::History::insert(pfad);
    } else {
        throw FileWriteException();
    }
}
void EplFile::speichernUnter(QString path)
{
    if (path == "")
        throw FilePathInvalidException();
    if (! FileIO::Schreibschutz::pruefen(path).isEmpty())
        throw FileWriteProtectedException();

    if (schreibeJsonInDatei(path, generiereJson(false))) {
        FileIO::Schreibschutz::setzen(path);
        if (!schreibgeschuetzt) {
            FileIO::Schreibschutz::freigeben(pfad);
        }
        schreibgeschuetzt = false;
        pfad = path;
        gespeichert = true;
        FileIO::History::insert(pfad);
    } else {
        throw FileWriteException();
    }
}
void EplFile::speichernPersonal(QString pfad)
{
    if (pfad == "")
        throw FilePathInvalidException();
    if (! FileIO::Schreibschutz::pruefen(pfad).isEmpty())
        throw FileWriteProtectedException();
    if (schreibeJsonInDatei(pfad, generiereJson(true))) {
        FileIO::History::insert(pfad);
    } else {
        throw FileWriteException();
    }
}
void EplFile::autoSave()
{
    if (schreibgeschuetzt) return;
    if (pfad == "") return;
    if (gespeichert) return;
    schreibeJsonInDatei(pfad+FileIO::getSuffixVonTyp(FileIO::DateiTyp::EPLAutoSave), generiereJson(false));
}

void EplFile::autoUpload()
{
    if (! dateiEigenschaften->getAutom()) {
        throw KeinAutoUploadException();
    }
    if (!Einstellungen::getUseAutoUpload()) {
        throw KeinAutoUploadException();
    }
    if (! dateiEigenschaften->getServer().isSecure()) {
        throw UnsichereVerbindungException();
    }
    Export::uploadToServer(manager->filter(dateiEigenschaften->getAuswahl()),
                           dateiEigenschaften->getServer());
}

void EplFile::open(QString passw)
{
    // HIER BEGINNT DAS EIGENTLICHE EINLESEN DER DATEN/DATEI
    QJsonObject generalJSON = geladen.value("general").toObject();
    Version version = Version(generalJSON.value("version").toString());
    QString payload = geladen.value("payload").toString();

    // Payload entschluesseln
    if (generalJSON.value("encrypted").toBool(false)) {
        if (! payload.endsWith("-ENC-"))
            throw FileException();
        payload.chop(5);
        QJsonObject cryptoJSON = geladen.value("crypto").toObject();
        EncryptedData eD = {payload, Crypto::hash(passw), cryptoJSON.value("salt").toString(), cryptoJSON.value("iv").toString(), cryptoJSON.value("modus").toString() };
        payload = Crypto::decrypt(eD);
        if (! payload.endsWith("-ZIP-"))
            throw FileWrongPasswordException();
    }
    if (passw == "")
        passwort = "";
    else
        passwort = Crypto::hash(passw);

    // Payload entpacken
    if (payload != "" || generalJSON.value("komprimiert").toBool(false)) {
        if (! payload.endsWith("-ZIP-"))
            throw FileException();
        payload.chop(5);
        geladen = dekomprimiere(payload);
    }

    // Daten anhand der angegebenen Version in aktuelles Format bringen
    switch(version.getMinor()) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    {
        QJsonObject calJson = geladen.value("calendar").toObject();

        personal = new ManagerPersonal(calJson.value("personal").toObject());
        manager = new Manager(personal, calJson.value("activites").toArray());

        currentDate = QDate::fromString(calJson.value("currentDate").toString(), "yyyy-MM-dd");
        break;
    }
    default:
        personal = new ManagerPersonal(geladen.value("personal").toObject());
        manager = new Manager(personal, geladen.value("activities").toArray());

        currentDate = QDate::fromString(geladen.value("view").toObject().value("currentDate").toString(), "yyyy-MM-dd");
        break;
    }

    dateiEigenschaften = new FileSettings(geladen.value("settings").toObject());

    QJsonObject viewJSON = geladen.value("view").toObject();
    int x = viewJSON.value("xMain").toInt();
    int y = viewJSON.value("yMain").toInt();
    int w = viewJSON.value("widthMain").toInt();
    int h = viewJSON.value("heightMain").toInt();
    positionKalender = FensterPosition {x, y, w, h};
    x = viewJSON.value("xPersonal").toInt();
    y = viewJSON.value("yPersonal").toInt();
    w = viewJSON.value("widthPersonal").toInt();
    h = viewJSON.value("heightPersonal").toInt();
    positionPersonal = FensterPosition {x, y, w, h};

    connect(manager, &Manager::changed, this, &EplFile::veraendern);
    connect(personal, &ManagerPersonal::changed, this, &EplFile::veraendern);
    connect(dateiEigenschaften, &FileSettings::changed, this, &EplFile::veraendern);

    // Schreibschutz pruefen
    QStringList l = FileIO::Schreibschutz::pruefen(pfad);
    if (!l.isEmpty()) {
        schreibgeschuetzt = true;
    } else {
        schreibgeschuetzt = false;
        FileIO::Schreibschutz::setzen(pfad);
    }
}

void EplFile::close()
{
    if (pfad != "") {
        QFile file (pfad+FileIO::getSuffixVonTyp(FileIO::DateiTyp::EPLAutoSave));
        file.remove();
    }
    if (!schreibgeschuetzt) {
        FileIO::Schreibschutz::freigeben(pfad);
    }
}


QJsonObject EplFile::generiereJson(bool staticOnly) const
{
    QJsonObject viewJSON = geladen.value("view").toObject();
    viewJSON.insert("xMain", positionKalender.x);
    viewJSON.insert("yMain", positionKalender.y);
    viewJSON.insert("widthMain", positionKalender.width);
    viewJSON.insert("heightMain", positionKalender.height);

    viewJSON.insert("xPersonal", positionPersonal.x);
    viewJSON.insert("yPersonal", positionPersonal.y);
    viewJSON.insert("widthPersonal", positionPersonal.width);
    viewJSON.insert("heightPersonal", positionPersonal.height);

    viewJSON.insert("currentDate", currentDate.toString("yyyy-MM-dd"));

    QJsonObject generalJSON = geladen.value("general").toObject();
    generalJSON.insert("version", Version::getProgrammVersion().toStringShort());

    QJsonObject object = QJsonObject();
    object.insert("view", viewJSON);
    object.insert("settings", dateiEigenschaften->toJson());
    object.insert("general", generalJSON);

    if (staticOnly) {
        object.insert("personal", personal->personalToJson());
    } else {
        object.insert("personal", personal->toJson());
        object.insert("activities", manager->toJson());
    }
    return object;
}

bool EplFile::schreibeJsonInDatei(QString pfad, QJsonObject obj) const
{
    QJsonObject zuschreibendesObjekt = obj;

    QJsonObject generalJson = zuschreibendesObjekt.value("general").toObject();

    // Daten komprimieren
    generalJson.insert("komprimiert", true);
    QString payload = komprimiere(zuschreibendesObjekt)+"-ZIP-";

    zuschreibendesObjekt = QJsonObject();

    if (hatPasswort()) {
        generalJson.insert("encrypted", true);

        EncryptedData eD = Crypto::encrypt(payload, passwort);
        QJsonObject crypto;
        crypto.insert("modus", eD.typ);
        crypto.insert("salt", eD.salt);
        crypto.insert("iv", eD.iv);
        zuschreibendesObjekt.insert("crypto", crypto);
        payload = eD.data+"-ENC-";
    }

    zuschreibendesObjekt.insert("general", generalJson);
    zuschreibendesObjekt.insert("payload", payload);

    return FileIO::saveJsonToFile(pfad, zuschreibendesObjekt);
}

QString EplFile::komprimiere(QJsonObject obj) const
{
    return QString(qCompress(QJsonDocument(obj).toJson(QJsonDocument::JsonFormat::Compact)).toBase64());
}
QJsonObject EplFile::dekomprimiere(QString komprimiert) const
{
    return QJsonDocument::fromJson(
                qUncompress(
                    QByteArray::fromBase64(komprimiert.toUtf8()
                        )
                    )
                ).object();
}

QString EplFile::getPasswort() const
{
    return passwort;
}
bool EplFile::setPasswort(const QString &neu, const QString &alt)
{
    if (passwort == "") {
        if (alt != "") {
            return false;
        }
    } else {
        if (Crypto::hash(alt) != passwort) {
            return false;
        }
    }
    if (neu == "") {
        passwort = "";
    } else {
        passwort = Crypto::hash(neu);
    }
    emit changed();
    return true;
}
bool EplFile::hatPasswort() const
{
    return passwort != "";
}
