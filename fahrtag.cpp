#include "fahrtag.h"

#include <QDate>
#include <QJsonArray>

Fahrtag::Fahrtag(QListWidgetItem *item, QListWidget *liste)
{
    this->item = item;
    this->liste = liste;

    QVariant v = qVariantFromValue((void *) this);
    item->setData(Qt::UserRole, v);

    datum = QDate::currentDate();
    zeitTf = QTime::fromString("08:15", "hh:mm");
    zeitZ = QTime::fromString("08:45", "hh:mm");
    art = 0;
    wichtig = false;
    wagen = "";
    anlass = "";
    tf = new QList<QString>();
    zf = new QList<QString>();
    zub = new QList<QString>();
    service = new QList<QString>();
    reservierungen = new ManagerReservierungen(liste);
    bemerkungen = "";

    benoetigeTf = true;
    benoetigeZf = true;
    benoetigeZub = true;
    benoetigeService = true;

    update();
}

Fahrtag::~Fahrtag()
{

}

bool Fahrtag::getBenoetigeTf() const
{
    return benoetigeTf;
}

void Fahrtag::setBenoetigeTf(bool value)
{
    benoetigeTf = value;
}

bool Fahrtag::getBenoetigeZf() const
{
    return benoetigeZf;
}

void Fahrtag::setBenoetigeZf(bool value)
{
    benoetigeZf = value;
}

bool Fahrtag::getBenoetigeZub() const
{
    return benoetigeZub;
}

void Fahrtag::setBenoetigeZub(bool value)
{
    benoetigeZub = value;
}

bool Fahrtag::getBenoetigeService() const
{
    return benoetigeService;
}

void Fahrtag::setBenoetigeService(bool value)
{
    benoetigeService = value;
}

// Wichtiges
void Fahrtag::update() {
    item->setText(getDatum().toString("dddd d. M. yyyy") + (getWichtig() ? " WICHTIG!" : ""));
    item->setToolTip(Anlass);
}

QJsonObject Fahrtag::toJson()
{
    QJsonObject o = QJsonObject();
    o.insert("datum", datum.toString("yyyy-M-d"));
    o.insert("art", art);
    o.insert("wichtig", wichtig);
    o.insert("anlass", anlass);
    o.insert("wagen", wagen);
    o.insert("tf", qListToJsonArray(tf));
    o.insert("zf", qListToJsonArray(zf));
    o.insert("zub", qListToJsonArray(zub));
    o.insert("bTf", benoetigeTf);
    o.insert("bZf", benoetigeZf);
    o.insert("bZub", benoetigeZub);
    o.insert("bService", benoetigeService);
    o.insert("service", qListToJsonArray(Service));
    o.insert("zeitTf", zeitTf.toString("hh:mm"));
    o.insert("zeitZ", zeitZ.toString("hh:mm"));
    o.insert("bemerkungen", Bemerkungen);
    o.insert("manager", reservierungen->toJson());
    return o;
}

QJsonArray Fahrtag::qListToJsonArray(QList<QString> *list) {
    QJsonArray a = QJsonArray();
    for (int i = 0; i < list->length(); i++) {
        a.append(list->at(i));
    }
    return a;
}
QList<QString> *Fahrtag::jsonArrayToQList(QJsonArray array) {
    QList<QString> *list = new QList<QString>();
    for (int i = 0; i < array.size(); i++) {
        list->append(array.at(i).toString());
    }
    return list;
}

void Fahrtag::fromJson(QJsonObject o)
{
    datum = QDate::fromString(o.value("datum").toString(), "yyyy-M-d");
    art = o.value("art").toInt();
    wichtig = o.value("wichtig").toBool();
    anlass = o.value("anlass").toString();
    wagen = o.value("wagen").toString();
    tf = jsonArrayToQList(o.value("tf").toArray());
    zf = jsonArrayToQList(o.value("zf").toArray());
    zub = jsonArrayToQList(o.value("zub").toArray());
    Service = jsonArrayToQList(o.value("service").toArray());
    sf (! o.value("bTf").isUndefined()) {
        benoetigeTf = o.value("bTf").toBool();
        benoetigeZf = o.value("bZf").toBool();
        benoetigeZub = o.value("bZub").toBool();
        benoetigeService = o.value("bService").toBool();
    }
    zeitTf = QTime::fromString(o.value("zeitTf").toString(), "hh:mm");
    zeitZ = QTime::fromString(o.value("zeitZ").toString(), "hh:mm");
    bemerkungen = o.value("bemerkungen").toString();
    reservierungen->fromJson(o.value("manager").toArray());
    update();
}


// GETTER
QDate Fahrtag::getDatum()
{
    return (datum);
}
int Fahrtag::getArt()
{
    return (art);
}
bool Fahrtag::getWichtig()
{
    return(wichtig);
}
QString Fahrtag::getAnlass()
{
    return(anlass);
}

QString Fahrtag::getWagenreihung()
{
    return(wagen);
}

QList<QString> *Fahrtag::getTf()
{
    return(tf);
}

QList<QString> *Fahrtag::getZf()
{
    return(zf);
}

QList<QString> *Fahrtag::getZub()
{
    return(zub);
}

QList<QString> *Fahrtag::getService()
{
    return(service);
}

QTime Fahrtag::getTimeTf()
{
    return(zeitTf);
}

QTime Fahrtag::getTimeZ()
{
    return(zeitZ);
}

QString Fahrtag::getBemerkungen()
{
    return(bemerkungen);
}

ManagerReservierungen *Fahrtag::getManager()
{
    return reservierungen;
}

QListWidgetItem *Fahrtag::getItem()
{
    return item;
}


// SETTER
void Fahrtag::setDatum(QDate datum)
{
    this->datum = datum;
}

void Fahrtag::setArt(int art)
{
    this->art = art;
}

void Fahrtag::setWichtig(bool wichtig)
{
    this->wichtig = wichtig;
}

void Fahrtag::setAnlass(QString anlass)
{
    this->anlass = anlass;
}

void Fahrtag::setWagenreihung(QString wagenreihung)
{
    wagen = wagenreihung;
}

void Fahrtag::setTf(QList<QString> *text)
{
    this->tf = text;
}

void Fahrtag::setZf(QList<QString> *text)
{
    this->zf = text;
}

void Fahrtag::setZub(QList<QString> *text)
{
    this->zub = text;
}

void Fahrtag::setService(QList<QString> *text)
{
    this->service = text;
}

void Fahrtag::setTimeTf(QTime zeit)
{
    this->zeitTf = zeit;
}

void Fahrtag::setTimeZ(QTime zeit)
{
    this->zeitZ = zeit;
}
void Fahrtag::setBemerkungen(QString text)
{
    this->bemerkungen = text;
}

void Fahrtag::setHidden(bool hide)
{
    item->setHidden(hide);
}
