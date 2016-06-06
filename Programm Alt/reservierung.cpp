#include "reservierung.h"

Reservierung::Reservierung(QListWidgetItem *item) {
    listItem = item;
    name = "(Neue Reservierung)";
    anzahl = 1;
    klasse = "2./3. Klasse";
    fahrrad = false;
    mail = "";
    telefon = "";
    autoPlatz = false;
    sitzplatz = "";
    sonstiges = "";
    startZug.insert(0, "2202 SOTW-SSWN");
    startHp.insert(0, "Ottweiler");
    endeZug.insert(0, "2203 SSWN-SOTW");
    endeHp.insert(0, "Ottweiler");
    update();
}


// METHODEN
void Reservierung::update()
{
    if (listItem != NULL)
        listItem->setText(name);
}

QJsonObject Reservierung::toJson() {
    QJsonObject object = QJsonObject();
    object.insert("name", name);
    object.insert("anzahl", anzahl);
    object.insert("klasse", klasse);
    object.insert("fahrrad", fahrrad);
    object.insert("mail", mail);
    object.insert("telefon", telefon);
    object.insert("startZug", startZug.at(0));
    object.insert("startHp", startHp.at(0));
    object.insert("endeZug", endeZug.at(0));
    object.insert("endeHp", endeHp.at(0));
    object.insert("autoplatz", autoPlatz);
    object.insert("sitzplatz", sitzplatz);
    object.insert("sonstiges", sonstiges);
    object.insert("sonstiges", sonstiges);

    return object;
}

void Reservierung::fromJson(QJsonObject json) {
    name = json.value("name").toString();
    anzahl = json.value("anzahl").toInt();
    klasse = json.value("klasse").toString();
    fahrrad = json.value("fahrrad").toBool();
    mail = json.value("mail").toString();
    telefon = json.value("telefon").toString();
    startZug.clear();
    startHp.clear();
    endeHp.clear();
    endeZug.clear();
    startZug.insert(0, json.value("startZug").toString());
    startHp.insert(0, json.value("startHp").toString());
    endeZug.insert(0, json.value("endeZug").toString());
    endeHp.insert(0, json.value("endeHp").toString());
    autoPlatz = json.value("autoplatz").toBool();
    sitzplatz = json.value("sitzplatz").toString();
    sonstiges = json.value("sonstiges").toString();
//    update();
}

// SETTER

void Reservierung::setName(const QString &value)
{
    name = value;
    update();
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

void Reservierung::setAutoPlatz(bool value)
{
    autoPlatz = value;
}

void Reservierung::setSitzplatz(const QString &value)
{
    sitzplatz = value;
}

void Reservierung::setSonstiges(const QString &value)
{
    sonstiges = value;
}

void Reservierung::setListItem(QListWidgetItem *value)
{
    listItem = value;
}



// GETTER

QString Reservierung::getName() const
{
    return name;
}

int Reservierung::getAnzahl() const
{
    return anzahl;
}

QString Reservierung::getKlasse() const
{
    return klasse;
}

bool Reservierung::getFahrrad() const
{
    return fahrrad;
}

QString Reservierung::getMail() const
{
    return mail;
}

QString Reservierung::getTelefon() const
{
    return telefon;
}

QList<QString> Reservierung::getStartZug() const
{
    return startZug;
}

QList<QString> Reservierung::getStartHp() const
{
    return startHp;
}

QList<QString> Reservierung::getEndeZug() const
{
    return endeZug;
}

QList<QString> Reservierung::getEndeHp() const
{
    return endeHp;
}

bool Reservierung::getAutoPlatz() const
{
    return autoPlatz;
}

QString Reservierung::getSitzplatz() const
{
    return sitzplatz;
}

QString Reservierung::getSonstiges() const
{
    return sonstiges;
}


