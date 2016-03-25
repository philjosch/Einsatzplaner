#include "fahrtag.h"

#include <QDate>

Fahrtag::Fahrtag(QListWidgetItem *item, QListWidget *liste)
{
    this->item = item;
    this->liste = liste;
    Datum = QDate::currentDate();
    ZeitTf = QTime::fromString("08:15", "hh:mm");
    ZeitZ = QTime::fromString("08:45", "hh:mm");
    Art = 0;
    Wichtig = false;
    wagen = "";
    QString Anlass = "";
    Tf = new QList<QString>();
    Zf = new QList<QString>();
    Zub = new QList<QString>();
    Service = new QList<QString>();
    reservierungen = new ManagerReservierungen(liste);
    update();
}

Fahrtag::~Fahrtag()
{

}

// Wichtiges
void Fahrtag::update() {
    item->setText(getDatum().toString("dddd d. M. yyyy") + (getWichtig() ? " WICHTIG!" : ""));
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

QString Fahrtag::getReservierungen()
{
    return(Reservierungen);
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
