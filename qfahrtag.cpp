#include "qfahrtag.h"
#include <QDate>

QFahrtag::QFahrtag(QListWidgetItem *listItem, QListWidget *liste)
{
    this->listItem = listItem;
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
}

QFahrtag::~QFahrtag()
{

}

QListWidgetItem *QFahrtag::getListItem() const
{
    return listItem;
}

void weiterleiten()
{

}

// GETTER
QDate QFahrtag::getDatum()
{
    return (Datum);
}
int QFahrtag::getArt()
{
    return (Art);
}
bool QFahrtag::getWichtig()
{
    return(Wichtig);
}
QString QFahrtag::getAnlass()
{
    return(Anlass);
}

QString QFahrtag::getWagenreihung()
{
    return(wagen);
}

QList<QString> *QFahrtag::getTf()
{
    return(Tf);
}

QList<QString> *QFahrtag::getZf()
{
    return(Zf);
}

QList<QString> *QFahrtag::getZub()
{
    return(Zub);
}

QList<QString> *QFahrtag::getService()
{
    return(Service);
}

QTime QFahrtag::getTimeTf()
{
    return(ZeitTf);
}

QTime QFahrtag::getTimeZ()
{
    return(ZeitZ);
}

QString QFahrtag::getReservierungen()
{
    return(Reservierungen);
}

QString QFahrtag::getBemerkungen()
{
    return(Bemerkungen);
}

ManagerReservierungen *QFahrtag::getManager()
{
    return reservierungen;
}


// SETTER
void QFahrtag::setDatum(QDate datum)
{
    Datum = datum;
}

void QFahrtag::setArt(int art)
{
    Art = art;
}

void QFahrtag::setWichtig(bool wichtig)
{
    Wichtig = wichtig;
}

void QFahrtag::setAnlass(QString anlass)
{
    Anlass = anlass;
}

void QFahrtag::setWagenreihung(QString wagenreihung)
{
    wagen = wagenreihung;
}

void QFahrtag::setTf(QList<QString> *text)
{
    Tf = text;
}

void QFahrtag::setZf(QList<QString> *text)
{
    Zf = text;
}

void QFahrtag::setZub(QList<QString> *text)
{
    Zub = text;
}

void QFahrtag::setService(QList<QString> *text)
{
    Service = text;
}

void QFahrtag::setTimeTf(QTime zeit)
{
    ZeitTf = zeit;
}

void QFahrtag::setTimeZ(QTime zeit)
{
    ZeitZ = zeit;
}
void QFahrtag::setBemerkungen(QString text)
{
    Bemerkungen = text;
}
