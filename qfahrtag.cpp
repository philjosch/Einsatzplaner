#include "qfahrtag.h"
#include <QDate>

QFahrtag::QFahrtag()
{
    Datum = QDate::currentDate();
    Art = 0;
    Wichtig = false;
    QString Anlass = "";

}

QFahrtag::~QFahrtag()
{

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

QString QFahrtag::getTextTf()
{
    return(Tf);
}

QString QFahrtag::getTextZf()
{
    return(Zf);
}

QString QFahrtag::getTextZub()
{
    return(Zub);
}

QString QFahrtag::getTextService()
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

void QFahrtag::setTextTf(QString text)
{
    Tf = text;
}

void QFahrtag::setTextZf(QString text)
{
    Zf = text;
}

void QFahrtag::setTextZub(QString text)
{
    Zub = text;
}

void QFahrtag::setTextService(QString text)
{
    Service = text;
}

void QFahrtag::setTimeTf(QTime zeit)
{
    ZeitTf = zeit;
}
