#include "eplexception.h"
#include "eplfile.h"
#include "fileio.h"

#include <QJsonArray>
#include <QJsonObject>
#include "basics.h"

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
}

EplFile::EplFile(QString path)
{
    pfad = path;
    gespeichert = true;

    if (path == "") {
        throw FilePathInvalidException();
    }

    // Daten aus Datei laden
    geladen = FileIO::getJsonFromFile(path);
    if (geladen.isEmpty()) {
        throw FileException();
    }
    // Schreibschutz pruefen
    QStringList l = FileIO::Schreibschutz::pruefen(pfad);
    if (!l.isEmpty()) {
        schreibgeschuetzt = true;
    } else {
        schreibgeschuetzt = false;
        FileIO::Schreibschutz::setzen(pfad);
    }

    // Prüfen, ob Version kompatibel ist
    QJsonObject generalJSON = geladen.value("general").toObject();
    QJsonArray activitiesJson;
    QJsonObject personalJson;

    Version version = Version::stringToVersion(generalJSON.value("version").toString());
    if (version == Version {-1, -1, -1}) {
        FileIO::Schreibschutz::freigeben(pfad);
        throw FileVersionNotSupportedException(tr("Die Datei ist nicht mit dieser Version des Einsatzplaner kompatibel."));
    } else if (version < Version {1, 6, -1}) {
        // Fallback fuer Version <1.6
        QJsonObject calendarJSON = geladen.value("calendar").toObject();
        activitiesJson = calendarJSON.value("activites").toArray();
        personalJson = calendarJSON.value("personal").toObject();
        // Daten in den Manager laden und die Logik herstellen
        currentDate = QDate::fromString(calendarJSON.value("currentDate").toString(), "yyyy-MM-dd");
    } else if (version <= Version::CURRENT_API_VERSION) {
        // Ab Version 1.6
        activitiesJson = geladen.value("activities").toArray();
        personalJson = geladen.value("personal").toObject();
        currentDate = QDate::fromString(geladen.value("view").toObject().value("currentDate").toString(), "yyyy-MM-dd");
    } else {
        // Version inkompatibel, zu neu
        FileIO::Schreibschutz::freigeben(pfad);
        throw FileToNewException(version);
    }

    manager = new Manager();
    personal = manager->getPersonal();
    personal->fromJson(personalJson);
    manager->fromJson(activitiesJson);
    dateiEigenschaften = new FileSettings();
    dateiEigenschaften->fromJson(geladen.value("settings").toObject());

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
    gespeichert = false;
}

EplFile::FensterPosition EplFile::getPositionKalender()
{
    return positionKalender;
}
void EplFile::setPositionKalender(const FensterPosition &value)
{
    positionKalender = value;
    gespeichert = false;
}

EplFile::FensterPosition EplFile::getPositionPersonal()
{
    return positionPersonal;
}
void EplFile::setPositionPersonal(const FensterPosition &value)
{
    positionPersonal = value;
    gespeichert = false;
}


void EplFile::speichern()
{
    if (schreibgeschuetzt) throw FileWriteProtectedException();
    if (pfad == "") throw  FilePathInvalidException();
    if (gespeichert) return;

    QJsonObject json = generiereJson();
    if (FileIO::saveJsonToFile(pfad, json)) {
        gespeichert = true;
        if (pfad != "") {
            QFile file (pfad+".autosave.ako");
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

    if (FileIO::saveJsonToFile(path, generiereJson())) {
        FileIO::Schreibschutz::setzen(path);
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
    if (FileIO::saveJsonToFile(pfad, generiereJsonPersonal())) {
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
    FileIO::saveJsonToFile(pfad+".autosave.ako", generiereJson());
}

void EplFile::close()
{
    if (pfad != "") {
        QFile file (pfad+".autosave.ako");
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
    generalJSON.insert("version", Version::CURRENT_API_VERSION.toStringShort());

    QJsonObject object = geladen;
    object.insert("personal", personalJSON);
    object.insert("view", viewJSON);
    object.insert("settings", dateiEigenschaften->toJson());
    object.insert("general", generalJSON);

    return object;
}

