#include "eplexception.h"
#include "eplfile.h"
#include "fileio.h"

#include <QJsonArray>
#include <QJsonObject>
#include "basics.h"

#include <QJsonDocument>
#include <exception>

EplFile::EplFile()
{
    pfad = "";
    schreibgeschuetzt = false;
    gespeichert = true;

    geladen = QJsonObject();

    manager = new Manager();
    personal = manager->getPersonal();
    dateiEigenschaften = new FileSettings();

    currentDate = QDate::currentDate();
    positionKalender = FensterPosition{};
    positionPersonal = FensterPosition{};

    connect(manager, &Manager::changed, this, &EplFile::veraendern);
    connect(personal, &ManagerPersonal::changed, this, &EplFile::veraendern);
    connect(dateiEigenschaften, &FileSettings::changed, this, &EplFile::veraendern);
}

EplFile::EplFile(QString path)
{
    pfad = path;
    gespeichert = true;

    if (path == "") {
        throw FilePathInvalidException();
    }

    // Schreibschutz pruefen
    QStringList l = FileIO::Schreibschutz::pruefen(pfad);
    if (!l.isEmpty()) {
        schreibgeschuetzt = true;
    } else {
        schreibgeschuetzt = false;
        FileIO::Schreibschutz::setzen(pfad);
    }

    QJsonArray activitiesJson;
    QJsonObject personalJson;

    geladen = leseJsonAusDatei(pfad);

    manager = new Manager();
    personal = manager->getPersonal();
    personal->fromJson(personalJson);
    manager->fromJson(activitiesJson);
    dateiEigenschaften = new FileSettings();
    dateiEigenschaften->fromJson(geladen.value("settings").toObject());
    personalJson = geladen.value("personal").toObject();
    activitiesJson = geladen.value("activities").toArray();

    currentDate = QDate::fromString(geladen.value("view").toObject().value("currentDate").toString(), "yyyy-MM-dd");

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
}


QString EplFile::getPfad() const
{
    return pfad;
}

bool EplFile::istSchreibgeschuetzt() const
{
    return schreibgeschuetzt;
}
QStringList EplFile::getInfoSchreibschutz()
{
    if (! schreibgeschuetzt) return QStringList();
    return FileIO::Schreibschutz::pruefen(pfad);
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

    QJsonObject json = generiereJson();
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

    if (schreibeJsonInDatei(path, generiereJson())) {
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
    if (schreibeJsonInDatei(pfad, generiereJsonPersonal())) {
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
    schreibeJsonInDatei(pfad+FileIO::getSuffixVonTyp(FileIO::DateiTyp::EPLAutoSave), generiereJson());
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


QJsonObject EplFile::generiereJson()
{
    QJsonObject json = generiereJsonPersonal();
    json.insert("activities", manager->toJson());
    return json;
}
QJsonObject EplFile::generiereJsonPersonal()
{
    QJsonObject personalJSON = manager->getPersonal()->personalToJson();

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

    QJsonObject object = geladen;
    object.insert("personal", personalJSON);
    object.insert("view", viewJSON);
    object.insert("settings", dateiEigenschaften->toJson());
    object.insert("general", generalJSON);

    return object;
}

bool EplFile::schreibeJsonInDatei(QString pfad, QJsonObject obj)
{
    if (Version::isDeveloperVersion()) {
        return FileIO::saveJsonToFile(pfad, obj);
    } else {
        QJsonObject generalJsonNeu;
        QJsonObject generalJsonAlt = obj.value("general").toObject();

        generalJsonNeu.insert("version", generalJsonAlt.value("version"));
        generalJsonNeu.insert("komprimiert", true);

        QJsonObject objectNeu;

        objectNeu.insert("komprimiert",
                         QString(qCompress(QJsonDocument(obj).toJson(QJsonDocument::JsonFormat::Compact)).toBase64())
                         );
        objectNeu.insert("general", generalJsonNeu);
        return FileIO::saveJsonToFile(pfad, objectNeu);
    }
}

QJsonObject EplFile::leseJsonAusDatei(QString pfad)
{
    // Daten aus Datei laden
    QJsonObject daten = FileIO::getJsonFromFile(pfad);
    if (daten.isEmpty()) {
        throw FileException();
    }

    // Pruefen, ob Version kompatibel ist
    QJsonObject generalJSON = daten.value("general").toObject();

    Version version = Version(generalJSON.value("version").toString());
    if (version == Version {-1, -1, -1}) {
        FileIO::Schreibschutz::freigeben(pfad);
        throw FileVersionNotSupportedException(tr("Die Datei ist nicht mit dieser Version des Einsatzplaner kompatibel."));
    }

    // Dateiversion zu neu abfangen
    if (version > Version::getProgrammVersion()) {
        // Version inkompatibel, zu neu
        FileIO::Schreibschutz::freigeben(pfad);
        throw FileToNewException(version);
    }

    // Dateiversion zu alt bzw. falsch eingestellt abfangen
    if (version.getMajor() < 1) {
        FileIO::Schreibschutz::freigeben(pfad);
        throw FileVersionNotSupportedException(tr("Die Datei ist nicht mit dieser Version des Einsatzplaner kompatibel."));
    }

    // Daten entpacken, sofern komprimiert
    if (generalJSON.value("komprimiert").toBool(false)) {
        daten = QJsonDocument::fromJson(
                    qUncompress(
                        QByteArray::fromBase64(
                            daten.value("komprimiert").toString().toUtf8()
                            )
                        )
                    ).object();
    }

    // Daten anhand der angegebenen Version in aktuelles Format bringen
    QJsonObject helper;
    switch(version.getMinor()) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        daten.insert("activities", daten.value("calendar").toObject().value("activites").toArray());
        daten.insert("personal",   daten.value("calendar").toObject().value("personal").toObject());

        helper = daten.value("view").toObject();
        helper.insert("currentDate", daten.value("calendar").toObject().value("currentDate").toString());
        daten.insert("view", helper);
        break;
    default:
        break;
    }
    return daten;
}

