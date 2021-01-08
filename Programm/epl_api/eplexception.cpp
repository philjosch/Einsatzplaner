#include "eplexception.h"

EplException::EplException()
{

}

EplException::EplException(QString msg)
{
    message = msg;
}

QString EplException::getError() const
{
    return message;
}
