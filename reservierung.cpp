#include "reservierung.h"

Reservierung::Reservierung() {

}

void Reservierung::setName(const QString &value)
{
    name = value;
}

void Reservierung::setAnzahl(int value)
{
    anzahl = value;
}

void Reservierung::setKlasse(const QString &value)
{
    klasse = value;
}

void Reservierung::setFahrrad(bool value)
{
    fahrrad = value;
}

void Reservierung::setMail(const QString &value)
{
    mail = value;
}

void Reservierung::setTelefon(const QString &value)
{
    telefon = value;
}

void Reservierung::setStartZug(const QString &value, int pos)
{
    startZug[pos] = value;
}

void Reservierung::setStartHp(const QString &value, int pos)
{
    startHp[pos] = value;
}

void Reservierung::setEndeZug(const QString &value, int pos)
{
    endeZug[pos] = value;
}

void Reservierung::setEndeHp(const QString &value, int pos)
{
    endeHp[pos] = value;
}

void Reservierung::setSitzplatz(const QString &value)
{
    sitzplatz = value;
}

void Reservierung::setSonstiges(const QString &value)
{
    sonstiges = value;
}
