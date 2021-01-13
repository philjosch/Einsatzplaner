#include "eplexception.h"

EplException::EplException()
{
    message = QObject::tr("Es ist ein unbekannter Fehler augetreten.");

}

EplException::EplException(QString msg)
{
    message = msg;
}

QString EplException::getError() const
{
    return message;
}
