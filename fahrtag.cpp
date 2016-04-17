#include "fahrtag.h"

#include <QDate>
#include <QJsonArray>

Fahrtag::Fahrtag(QListWidgetItem *item, QListWidget *liste)
{
    this->item = item;
    this->liste = liste;

    QVariant v = qVariantFromValue((void *) this);
    item->setData(Qt::UserRole, v);


    Datum = QDate::currentDate();
    ZeitTf = QTime::fromString("08:15", "hh:mm");
    ZeitZ = QTime::fromString("08:45", "hh:mm");
    Art = 0;
    Wichtig = false;
    wagen = "";
    Anlass = "";
    Tf = new QList<QString>();
    Zf = new QList<QString>();
    Zub = new QList<QString>();
    Service = new QList<QString>();
    reservierungen = new ManagerReservierungen(liste);

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
    o.insert("datum", Datum.toString("yyyy-M-d"));
    o.insert("art", Art);
    o.insert("wichtig", Wichtig);
    o.insert("anlass", Anlass);
    o.insert("wagen", wagen);
    o.insert("tf", qListToJsonArray(Tf));
    o.insert("zf", qListToJsonArray(Zf));
    o.insert("zub", qListToJsonArray(Zub));
    o.insert("bTf", benoetigeTf);
    o.insert("bZf", benoetigeZf);
    o.insert("bZub", benoetigeZub);
    o.insert("bService", benoetigeService);
    o.insert("service", qListToJsonArray(Service));
    o.insert("zeitTf", ZeitTf.toString("hh:mm"));
    o.insert("zeitZ", ZeitZ.toString("hh:mm"));
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
    Datum = QDate::fromString(o.value("datum").toString(), "yyyy-M-d");
    Art = o.value("art").toInt();
    Wichtig = o.value("wichtig").toBool();
    Anlass = o.value("anlass").toString();
    wagen = o.value("wagen").toString();
    Tf = jsonArrayToQList(o.value("tf").toArray());
    Zf = jsonArrayToQList(o.value("zf").toArray());
    Zub = jsonArrayToQList(o.value("zub").toArray());
    Service = jsonArrayToQList(o.value("service").toArray());
    if (! o.value("bTf").isUndefined()) {
        benoetigeTf = o.value("bTf").toBool();
        benoetigeZf = o.value("bZf").toBool();
        benoetigeZub = o.value("bZub").toBool();
        benoetigeService = o.value("bService").toBool();
    }
    ZeitTf = QTime::fromString(o.value("zeitTf").toString(), "hh:mm");
    ZeitZ = QTime::fromString(o.value("zeitZ").toString(), "hh:mm");
    Bemerkungen = o.value("bemerkungen").toString();
    if (o.value("manager").isArray())
        reservierungen->fromJson(o.value("manager").toArray());
    else
        reservierungen->fromJson(o.value("manager").toObject());
    update();
}

void Fahrtag::verteileSitzplaetze()
{

    QStringList l = wagen.split(",");
    QList<Wagen*> *andereKlasse = new QList<Wagen*>();
    QList<Wagen*> *ersteKlasse = new QList<Wagen*>();
    foreach (QString w, l) {
        switch (Wagen::klasse(w.toInt())) {
        case 1: ersteKlasse->append(new Wagen(w.toInt())); break;
        case 2: andereKlasse->append(new Wagen(w.toInt())); break;
        case 3: andereKlasse->append(new Wagen(w.toInt())); break;
        default: break;
        }
    }
    reservierungen->verteileSitzplaetze(ersteKlasse, andereKlasse);
}




// GETTER
QDate Fahrtag::getDatum()
{
    return (Datum);
}
int Fahrtag::getArt()
{
    return (Art);
}
bool Fahrtag::getWichtig()
{
    return(Wichtig);
}
QString Fahrtag::getAnlass()
{
    return(Anlass);
}

QString Fahrtag::getWagenreihung()
{
    return(wagen);
}

QList<Wagen *> *Fahrtag::getErsteKlasseWagen()
{
    QStringList l = wagen.split(",");
    QList<Wagen*> *wa = new QList<Wagen*>();
    foreach (QString w, l) {
        if (Wagen::klasse(w.toInt()) == 1)
            wa->append(new Wagen(w.toInt()));
    }
    return wa;
}

QList<Wagen *> *Fahrtag::getAndereKlasseWagen()
{
    QStringList l = wagen.split(",");
    QList<Wagen*> *wa = new QList<Wagen*>();
    foreach (QString w, l) {
        if (Wagen::klasse(w.toInt()) == 1)
            wa->append(new Wagen(w.toInt()));
    }
    return wa;
}

QList<QString> *Fahrtag::getTf()
{
    return(Tf);
}

QList<QString> *Fahrtag::getZf()
{
    return(Zf);
}

QList<QString> *Fahrtag::getZub()
{
    return(Zub);
}

QList<QString> *Fahrtag::getService()
{
    return(Service);
}

QTime Fahrtag::getTimeTf()
{
    return(ZeitTf);
}

QTime Fahrtag::getTimeZ()
{
    return(ZeitZ);
}

QString Fahrtag::getBemerkungen()
{
    return(Bemerkungen);
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
    Datum = datum;
}

void Fahrtag::setArt(int art)
{
    Art = art;
}

void Fahrtag::setWichtig(bool wichtig)
{
    Wichtig = wichtig;
}

void Fahrtag::setAnlass(QString anlass)
{
    Anlass = anlass;
}

void Fahrtag::setWagenreihung(QString wagenreihung)
{
    wagen = wagenreihung;
}

void Fahrtag::setTf(QList<QString> *text)
{
    Tf = text;
}

void Fahrtag::setZf(QList<QString> *text)
{
    Zf = text;
}

void Fahrtag::setZub(QList<QString> *text)
{
    Zub = text;
}

void Fahrtag::setService(QList<QString> *text)
{
    Service = text;
}

void Fahrtag::setTimeTf(QTime zeit)
{
    ZeitTf = zeit;
}

void Fahrtag::setTimeZ(QTime zeit)
{
    ZeitZ = zeit;
}
void Fahrtag::setBemerkungen(QString text)
{
    Bemerkungen = text;
}

void Fahrtag::setHidden(bool hide)
{
    item->setHidden(hide);
}
