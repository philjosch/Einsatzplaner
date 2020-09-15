#include "personalwindow.h"
#include "managerpersonal.h"
#include "person.h"

#include <QJsonArray>
#include <QRandomGenerator>
#include <algorithm>
#include <cmath>

const QString Person::FARBE_FEHLENDE_STUNDEN = "#ff9999";
const QString Person::FARBE_GENUG_STUNDEN = "#99ff99";
const QString Person::FARBE_STANDARD = "#ffffff";

Person::Person(QString name, ManagerPersonal *manager) : QObject()
{
    name = ManagerPersonal::getGoodName(name);
    QString vorname = "";
    QString nachname = "";
    if (! name.contains(" ")) {
        nachname = name;
    } else {
        QStringList liste = name.split(" ");
        for(int i = 0; i < liste.length()-1; i++) {
            vorname += liste.at(i);
            if (i < liste.length() - 2) {
                vorname += " ";
            }
        }
        nachname = liste.last();
    }
    personConstructor(vorname, nachname, manager);
}

Person::Person(QJsonObject o, ManagerPersonal *man) : QObject()
{
    additional = QMap<Category, int>();
    zeiten = QMap<Category, int>();
    activities = QMap<AActivity *, Category>();

    valuesInvalid = true;
    manager = man;

    if (o.contains("id")) {
        id = o.value("id").toString();
    } else {
        id = QString::number(QDateTime::currentDateTime().toSecsSinceEpoch())+QString::number(QRandomGenerator::global()->bounded(1000000,9999999));
    }
    if (o.contains("vorname") && o.contains("nachname")) {
        vorname = o.value("vorname").toString();
        nachname = o.value("nachname").toString();
    } else {
        QStringList liste = o.value("name").toString().split(" ");
        for(int i = 0; i < liste.length()-1; i++) {
            vorname += liste.at(i);
            if (i < liste.length() - 2) {
                vorname += " ";
            }
        }
        nachname = liste.last();
    }

    if (o.contains("nummer")) {
        nummer = o.value("nummer").toInt();
    } else if (manager != nullptr) {
        nummer = manager->getNextNummer();
    } else {
        nummer = 1;
    }

    geburtstag = QDate::fromString(o.value("geburtstag").toString(), "yyyy-MM-dd");
    eintritt = QDate::fromString(o.value("eintritt").toString(), "yyyy-MM-dd");
    aktiv = o.value("aktiv").toBool(true);
    austritt = QDate::fromString(o.value("austritt").toString(), "yyyy-MM-dd");

    ausbildungRangierer = o.value("ausRang").toBool();
    ausbildungZf = o.value("ausZf").toBool();
    ausbildungTf = o.value("ausTf").toBool();
    tauglichkeit = QDate::fromString(o.value("tauglichkeit").toString(), "yyyy-MM-dd");

    strasse = o.value("strasse").toString();
    plz = o.value("plz").toString();
    ort = o.value("ort").toString();
    mail = o.value("mail").toString();
    mailOK = o.value("mailOK").toBool(true);
    telefon = o.value("tel").toString();
    telefonOK = o.value("telOK").toBool(true);

    strecke = o.value("strecke").toInt();
    beruf= o.value("beruf").toString();
    bemerkungen = o.value("bemerkung").toString();
    if (o.contains("additionalKeys") && o.contains("additionalValues")) {
        QJsonArray keys = o.value("additionalKeys").toArray();
        QJsonArray values = o.value("additionalValues").toArray();
        for (int i = 0; i < keys.size() && i < values.size() ; i++) {
            additional.insert(Category(keys.at(i).toInt()), values.at(i).toInt());
        }
    } else {
        // Fallback für ältere Versionen des Programms
        setAdditional(Tf, int(o.value("additionalTf").toDouble(0)*60));
        setAdditional(Zf, int(o.value("additionalZf").toDouble(0)*60));
        setAdditional(Zub, int(o.value("additionalZub").toDouble(0)*60));
        setAdditional(Buero, int(o.value("additionalBuero").toDouble(0)*60));
        setAdditional(Service, int(o.value("additionalService").toDouble(0)*60));
        setAdditional(Werkstatt, int(o.value("additionalWerkstatt").toDouble(0)*60));
        setAdditional(ZugVorbereiten, int(o.value("additionalVorbereiten").toDouble(0)*60));
        setAdditional(Ausbildung, int(o.value("additionalAusbildung").toDouble(0)*60));
        setAdditional(Sonstiges, int(o.value("additionalSonstiges").toDouble(0)*60));
        setAdditional(Anzahl, o.value("additionalAnzahl").toInt());
        setAdditional(Kilometer, o.value("additionalKilometer").toInt());
    }
}

void Person::personConstructor(QString vn, QString nn, ManagerPersonal *man, QString ID)
{
    additional = QMap<Category, int>();
    zeiten = QMap<Category, int>();
    activities = QMultiMap<AActivity *, Category>();

    valuesInvalid = true;
    manager = man;

    if (ID == "")
        id = QString::number(QDateTime::currentDateTime().toSecsSinceEpoch())+QString::number(QRandomGenerator::global()->bounded(1000000,9999999));
    else
        id = ID;

    if (manager == nullptr)
        nummer = 1;
    else
        nummer = manager->getNextNummer();
    vorname = vn;
    nachname = nn;
    geburtstag = QDate();
    eintritt = QDate::currentDate();
    aktiv = true;
    austritt = QDate();

    ausbildungTf = false;
    ausbildungZf = false;
    ausbildungRangierer = false;
    tauglichkeit = QDate();

    strasse = "";
    plz = "";
    ort = "";
    mail = "";
    mailOK = true;
    telefon = "";
    telefonOK = true;
    // Sonstiges
    strecke = 0;
    beruf = "";
    bemerkungen = "";
}

QJsonObject Person::toJson()
{
    QJsonObject o = personalToJson();
    // Zusätzliche Zeiten einfügen
    QJsonArray keys;
    QJsonArray values;
    foreach (Category cat, additional.keys()) {
        if (additional.value(cat) > 0) {
            keys.append(int(cat));
            values.append(additional.value(cat));
        }
    }
    o.insert("additionalKeys", keys);
    o.insert("additionalValues", values);
    return o;
}

QJsonObject Person::personalToJson()
{
    QJsonObject o;
    o.insert("id", id);
    o.insert("nummer", nummer);
    o.insert("vorname", vorname);
    o.insert("nachname", nachname);
    o.insert("geburtstag", geburtstag.toString("yyyy-MM-dd"));
    o.insert("eintritt", eintritt.toString("yyyy-MM-dd"));
    o.insert("aktiv", aktiv);
    o.insert("austritt", austritt.toString("yyyy-MM-dd"));

    o.insert("ausTf", ausbildungTf);
    o.insert("ausZf", ausbildungZf);
    o.insert("ausRang", ausbildungRangierer);
    o.insert("tauglichkeit", tauglichkeit.toString("yyyy-MM-dd"));

    o.insert("strasse", strasse);
    o.insert("plz", plz);
    o.insert("ort", ort);
    o.insert("mail", mail);
    o.insert("mailOK", mailOK);
    o.insert("tel", telefon);
    o.insert("telOK", telefonOK);

    o.insert("strecke", strecke);
    o.insert("beruf", beruf);
    o.insert("bemerkung", bemerkungen);
    return o;
}

int Person::getZeiten(Category cat)
{
    if (valuesInvalid) berechne();
    switch (cat) {
    case Category::Begleiter:
        return zeiten.value(Zub, 0);
    default:
        return zeiten.value(cat, 0);
    }
}

bool Person::getAusbildungTf() const
{
    return ausbildungTf;
}
void Person::setAusbildungTf(bool value)
{
    ausbildungTf = value;
    emit changed();
}

bool Person::getAusbildungZf() const
{
    return ausbildungZf;
}
void Person::setAusbildungZf(bool value)
{
    ausbildungZf = value;
    emit changed();
}

bool Person::getAusbildungRangierer() const
{
    return ausbildungRangierer;
}
void Person::setAusbildungRangierer(bool value)
{
    ausbildungRangierer = value;
    emit changed();
}

int Person::getStrecke() const
{
    return strecke;
}
void Person::setStrecke(int value)
{
    strecke = value;
    valuesInvalid = true;
    emit changed();
}

QString Person::getMail() const
{
    return mail;
}
void Person::setMail(const QString &value)
{
    mail = value;
    emit changed();
}

bool Person::getMailOK() const
{
    return mailOK;
}
void Person::setMailOK(bool value)
{
    mailOK = value;
    emit changed();
}

QString Person::getTelefon() const
{
    return telefon;
}
void Person::setTelefon(const QString &value)
{
    telefon = value;
    emit changed();
}

bool Person::getTelefonOK() const
{
    return telefonOK;
}
void Person::setTelefonOK(bool value)
{
    telefonOK = value;
    emit changed();
}

QString Person::getBeruf() const
{
    return beruf;
}
void Person::setBeruf(const QString &value)
{
    beruf = value;
    emit changed();
}

QString Person::getBemerkungen() const
{
    return bemerkungen;
}
void Person::setBemerkungen(const QString &value)
{
    bemerkungen = value;
    emit changed();
}

int Person::getNummer() const
{
    return nummer;
}
bool Person::setNummer(int value)
{
    if (manager->checkNummer(value)) {
        nummer = value;
        emit changed();
        return true;
    }
    return false;
}

QString Person::getPLZ() const
{
    return plz;
}
void Person::setPLZ(const QString &value)
{
    plz = value;
    emit changed();
}

QString Person::getOrt() const
{
    return ort;
}
void Person::setOrt(const QString &value)
{
    ort = value;
    emit changed();
}

QString Person::getStrasse() const
{
    return strasse;
}
void Person::setStrasse(const QString &value)
{
    strasse = value;
    emit changed();
}

bool Person::isTauglich(Category cat, QDate datum)
{
    switch(cat) {
    case Tf:
        if (! ausbildungTf) return false;
        break;
    case Tb:
        if (! ausbildungTf && !ausbildungZf) return false;
        break;
    case Zf:
        if (! ausbildungZf) return false;
        break;
    case Zub:
        if (! ausbildungRangierer) return false;
        break;
    default:
        return true;
    }
    if (tauglichkeit.isNull()) return true;
    return (tauglichkeit >= datum);
}

bool Person::isAusgetreten()
{
    if (! austritt.isValid()) return false;
    return (austritt <= QDate::currentDate());
}

bool Person::pruefeFilter(Mitglied filter)
{
    switch (filter) {
    case AlleMitglieder:
        return (! isAusgetreten());
    case Aktiv:
        return ((! isAusgetreten()) && getAktiv());
    case Passiv:
        return ((! isAusgetreten()) && (! getAktiv()));
    case AktivMit:
        return ((! isAusgetreten()) && getAktiv() && (pruefeStunden(Gesamt) == AktivMit));
    case AktivOhne:
        return ((! isAusgetreten()) && getAktiv() && (pruefeStunden(Gesamt) == AktivOhne));
    case PassivMit:
        return ((! isAusgetreten()) && (!getAktiv()) && (pruefeStunden(Gesamt) == PassivMit));
    case PassivOhne:
        return ((! isAusgetreten()) && (!getAktiv()) && (pruefeStunden(Gesamt) != PassivMit));
    case Ausgetreten:
        return (isAusgetreten());
    case Registriert:
        return true;
    }
}

Mitglied Person::pruefeStunden()
{
    if (isAusgetreten()) return Mitglied::Ausgetreten;
    if (! getAktiv()) {
        if (getZeiten(Gesamt) > 0)
            return Mitglied::PassivMit;
        else return Mitglied::Passiv;
    }
    foreach (Category cat, ANZEIGEREIHENFOLGEGESAMT) {
        if (pruefeStunden(cat) == AktivOhne) {
            return Mitglied::AktivOhne;
        }
    }
    return Mitglied::AktivMit;
}

Mitglied Person::pruefeStunden(Category cat)
{
    if (isAusgetreten()) return Mitglied::Ausgetreten;
    if (! getAktiv()) {
        if (getZeiten(cat) > 0)
            return Mitglied::PassivMit;
        else return Mitglied::Passiv;
    }

    if (getZeiten(cat) >= getMinimumStunden(cat))
        return Mitglied::AktivMit;
    else return Mitglied::AktivOhne;
}

int Person::getMinimumStunden(Category cat)
{
    if (isMinderjaehrig()) return 0;
    if (isAusgetreten()) return 0;
    switch (cat) {
    case Tf:
        if (! isTauglich(Tf) ) return 0;
        return manager->getMinimumHours(cat);
    case Zf:
        if (! isTauglich(Zf)) return 0;
        return manager->getMinimumHours(cat);
    case Ausbildung:
        if ((! getAusbildungTf() && ! getAusbildungZf() && ! getAusbildungRangierer()) || !isTauglich()) return 0;
        return manager->getMinimumHours(cat);
    default:
        return manager->getMinimumHours(cat);
    }
}

void Person::berechne()
{
    zeiten.clear();
    zeiten.insert(Anzahl, activities.size());
    
    for(AActivity *a: activities.uniqueKeys()) {
        if (a->getDatum() > QDate::currentDate()) continue;
        for(Category cat: activities.values(a)) {
            Infos info = a->getIndividual(this, cat);

            // Einsatzstunden
            QTime start = info.beginn;
            QTime ende = info.ende;
            int duration = (start.msecsTo(ende) / 60000); // in Minuten
            switch (cat) {
            case Begleiter:
                zeiten.insert(Zub, zeiten.value(Zub)+duration);
                break;
            case Anzahl: break;
            case Kilometer: break;
            case Gesamt: break;
            default:
                zeiten.insert(cat, zeiten.value(cat)+duration);
                break;
            }
            zeiten.insert(Gesamt, zeiten.value(Gesamt)+duration);
            if (cat != Category::Buero)
                zeiten.insert(Kilometer, zeiten.value(Kilometer)+2*strecke);
        }
    }
    foreach (Category cat, additional.keys()) {
       zeiten.insert(cat, zeiten.value(cat)+additional.value(cat));
       switch (cat) {
       case Anzahl: break;
       case Gesamt: break;
       case Kilometer: break;
       default:
           zeiten.insert(Gesamt, zeiten.value(Gesamt)+additional.value(cat));
       }
    }
    valuesInvalid = false;
}

bool Person::addActivity(AActivity *a, Category kat)
{
    if (!activities.values(a).contains(kat)) {
        activities.insert(a, kat);

        valuesInvalid = true;
        return true;
    } else {
        return false;
    }
}

bool Person::removeActivity(AActivity *a, Category kat)
{
    QList<Category> category = activities.values(a);
    activities.remove(a);
    if (category.removeAll(kat)) {
        for(Category c: category) {
            activities.insert(a, c);
        }
        valuesInvalid = true;
        return true;
    }
    return false;
}

QMultiMap<AActivity *, Category> Person::getActivities()
{
    return activities;
}

QString Person::getName() const
{
    QString nameKomplett;
    if (vorname != "" && nachname != "") return vorname + " " + nachname;
    else if (vorname != "") return vorname;
    else return nachname;
}

QString Person::getVorname() const
{
    return vorname;
}
void Person::setVorname(const QString &value)
{
    QString old = getName();
    vorname = value;
    while (vorname.endsWith(" ")) vorname.chop(1);
    while (vorname.startsWith(" ")) vorname = vorname.remove(0, 1);
    emit nameChanged(this, old);
    emit changed();
}

QString Person::getNachname() const
{
    return nachname;
}
void Person::setNachname(const QString &value)
{
    QString old = getName();
    nachname = value;
    while (nachname.endsWith(" ")) nachname.chop(1);
    while (nachname.startsWith(" ")) nachname = nachname.remove(0, 1);
    emit nameChanged(this, old);
    emit changed();
}

QString Person::getZeitenFuerListeAlsHTML(QSet<Category> liste)
{
    QString html = "<tr><td style='background-color:";
    switch (pruefeStunden()) {
    case AktivOhne:  html += FARBE_FEHLENDE_STUNDEN; break;
    case PassivMit: html += FARBE_GENUG_STUNDEN; break;
    default: html += FARBE_STANDARD;
    }
    html += "'>"+getName()+"</td>";

    foreach(Category cat, ANZEIGEREIHENFOLGEGESAMT) {
        if (!liste.contains(cat)) continue;
        html += "<td align='right' style='background-color: ";
        switch (pruefeStunden(cat)) {
        case AktivOhne:  html += Person::FARBE_FEHLENDE_STUNDEN; break;
        case PassivMit: html += Person::FARBE_GENUG_STUNDEN; break;
        default: html += Person::FARBE_STANDARD;
        }
        if (getZeiten(cat) > 0) {
            if (cat == Kilometer || cat == Anzahl) {
                html += QString("'>%1</td>").arg(getZeiten(cat));
            } else {
                html += QString("'>%1</td>").arg(minutesToHourStringShort(getZeiten(cat)));
            }
        } else {
            html += "'></td>";
        }
    }
    html += "</tr>";
    return html;
}

QString Person::getZeitenFuerEinzelAlsHTML()
{
    berechne();
    QString html = "<h2>"+vorname+" "+nachname+"</h2>";
    if (ausbildungRangierer || ausbildungTf || ausbildungZf)
        html = html + "<p>Ausbildung und Tauglichkeit als: " + (isTauglich(Tf)? "Triebfahrzeugführer, ":"") + (isTauglich(Zf)? "Zugführer, ":"")
                + (isTauglich(Zub)? "Rangierer":" ") + "<br/>";
    html += "Entfernung zum Bahnhof: "+QString::number(strecke) + "km</p>";
    html += "<h3>Geleistete Stunden</h3>";
    html += "<ul>";
    QString help = "<li %1>%3: %4%2</li>";
    QString helpcurrent;
    foreach (Category cat, ANZEIGEREIHENFOLGE) {
        if (getZeiten(cat) > 0 || getMinimumStunden(cat) > 0) {
            if (pruefeStunden(cat) != Mitglied::AktivOhne) helpcurrent = help.arg("", "");
            else helpcurrent = help.arg("style=\"color: red;\"", " (mindestens "+minutesToHourString(getMinimumStunden(cat))+")");
            html += helpcurrent.arg(getLocalizedStringFromCategory(cat), minutesToHourString(getZeiten(cat)));
        }
    }
    html += "</ul><br/><ul>";
    if (pruefeStunden(Gesamt) != Mitglied::AktivOhne) helpcurrent = help.arg("", "");
    else helpcurrent = help.arg("style=\"color: red;\"", " (mindestens "+minutesToHourString(getMinimumStunden(Gesamt))+")");
    html += helpcurrent.arg("Gesamte Stundenzahl", minutesToHourString(getZeiten(Gesamt)));
    html += "<li>Anzahl Aktivitäten: "+QString::number(getZeiten(Anzahl))+"</li>";
    html += "<li>Gefahrene Strecke: "+QString::number(getZeiten(Kilometer))+"km</li></ul>";
    html += "</p>";
    html += QString("<p><br/>%1 %2 hat die notwendigen Stunden %3erbracht!</p>").arg(vorname, nachname)
            .arg(pruefeStunden() != AktivOhne ? "" : "<b style=\"color: red;\">nicht</b> ");

    // Hier kommt die liste mit den Arbeitseinsätzen
    html += "<h3>Übersicht über die einzelnen Aktivitäten</h3>";
    if (activities.size() > 0) {
        html += "<table cellspacing='0' width='100%'><thead>";
        html += "<tr><th>Datum, Anlass</th><th>Dienstzeiten</th><th>Aufgabe</th><th>Bemerkung</th></tr></thead><tbody>";
        QList<AActivity*> list = activities.uniqueKeys();
        AActivity::sort(&list);
        for(int i=0; i<list.length(); i++) {
            AActivity *a = list.at(i);
            for(Category cat: activities.values(a)) {
                Infos info = a->getIndividual(this, cat);
                html += "<tr><td>"+a->getDatum().toString("dd.MM.yyyy")+"<br/>"+a->getKurzbeschreibung();
                if (a->getAnlass() != a->getKurzbeschreibung() && a->getAnlass() != "")
                    html += "<br/>"+a->getAnlass();
                html +="</td><td>"
                     + info.beginn.toString("HH:mm")+"-"+info.ende.toString("HH:mm")+"</td><td>"
                     + getLocalizedStringFromCategory(info.kategorie) + "</td><td>"
                     + info.bemerkung + "</td></tr>";
            }
        }
        html += "</tbody></table>";
    } else {
        html += QString("<p>%1 %2 hat an keinen eingetragenen Aktivitäten teilgenommen!</p>").arg(vorname, nachname);
    }
    help = "<li>%1: %2</li>";
    QString h2 = "";
    foreach (Category cat, ANZEIGEREIHENFOLGE) {
        if (getAdditional(cat) > 0)
            h2 += help.arg(getLocalizedStringFromCategory(cat), minutesToHourString(getAdditional(cat)));
    }
    help = "<li>%1: %2%3</li>";
    if (getAdditional(Anzahl) > 0)
        h2 += help.arg("Zusätzliche Aktivitäten").arg(getAdditional(Anzahl)).arg("");
    if (getAdditional(Kilometer) > 0)
        h2 += help.arg("Gefahrene Strecke").arg(getAdditional(Kilometer)).arg("km");
    if (h2 != "")
        html+= "<h3>Zusätzliche nicht in der Tabelle erfassten Stunden</h3><ul>"+h2+"</ul>";
    return html;
}

QString Person::getPersonaldatenFuerListeAlsHTML()
{
    QString h = "<tr>";
    h += "<td>"+getName()+"<br/>"
            +QString::number(nummer)+"<br/>";
    if (isAusgetreten())
        h += "Ehemals: ";
    h = h + (aktiv ? "Aktiv":"Passiv")+"</td>";

    // Daten
    h += "<td>"+geburtstag.toString("dd.MM.yyyy")+"<br/>"+eintritt.toString("dd.MM.yyyy");
    if (isAusgetreten()) {
        h += austritt.toString("-dd.MM.yyyy");
    } else {
        if (!austritt.isNull())
            h += "<br/>Austritt zum: "+austritt.toString("dd.MM.yyyy");
    }
    if (beruf != "")
        h += "<br/>"+beruf;
    h += "</td>";

    // Anschrift
    h += "<td>"+strasse + "<br/>"+plz + " "+ ort;
    if (strecke > 0) {
        h += "<br/>"+QString::number(strecke)+"km";
    }
    h += "</td>";

    // Kontakt
    h += "<td>";
    if (mail != "") {
        if (mailOK)
            h += mail;
        else
            h += "("+mail+")";
    }
    h += "<br/>";
    if (telefon != "") {
        if (telefonOK)
            h += telefon;
        else
            h += "("+telefon+")";
    }
    h += "</td>";

    // Betriebsdienst
    h = h + "<td>"+ (ausbildungTf? "Triebfahrzeugführer<br/>":"")
            + (ausbildungZf? "Zugführer<br/>":"")
            + (ausbildungRangierer? "Rangierer<br/>":"")
            + tauglichkeit.toString("dd.MM.yyyy")
            + "</td>";

    // Sonstiges
    h += "<td>" + bemerkungen + "</td>";

    h += "</tr>";
    return h;
}

QString Person::getPersonaldatenFuerEinzelAlsHTML()
{
    QString h = "<h2>"+vorname+" "+nachname+"</h2>";

    QString help = "%1:\t\t %2<br/>";
    h += "<h3>Stammdaten</h3><p>";
    h += help.arg("Mitgliedsnummer").arg(nummer);
    h += help.arg("Status").arg( isAusgetreten() ? "Ehemals %1" : "%1")
            .arg(aktiv ? "Aktiv":"Passiv");

    // Daten
    h += help.arg("Geburtstag").arg(geburtstag.toString("dd.MM.yyyy"));
    h += help.arg("Eintritt").arg(eintritt.toString("dd.MM.yyyy"));
    if (isAusgetreten()) {
        h += help.arg("Austritt").arg(austritt.toString("dd.MM.yyyy"));
    } else {
        if (austritt.isValid())
            h += "<br/>Austritt zum: "+austritt.toString("dd.MM.yyyy");
    }
    h += help.arg("Beruf").arg(beruf);

    h += "</p><h3>Anschrift</h3><p>";

    // Anschrift
    h += help.arg("Straße").arg(strasse);
    h += help.arg("PLZ").arg(plz);
    h += help.arg("Ort").arg(ort);
    h += help.arg("Strecke").arg("%1km").arg(strecke);

    h += "</p><h3>Kontaktdaten</h3><p>";

    // Kontakt
    h += help.arg("Mail (darf %3weitergegeben werden)").arg(mail).arg(mailOK ? "" : "nicht");
    h += help.arg("Telefon (darf %3weitergegeben werden)").arg(telefon).arg(telefonOK ? "" : "nicht");
    h += "</p>";

    if (ausbildungRangierer || ausbildungTf || ausbildungZf || tauglichkeit.isValid()) {
        h += "<h3>Betriebsdienst</h3><p>";
        h += help.arg("Tauglichkeit bis").arg(tauglichkeit.toString("dd.MM.yyyy"));
        h += help.arg("Ausbildung zum").arg("") + "</p><ul>";
        h += (ausbildungTf? "<li>Triebfahrzeugführer</li>": "");
        h += (ausbildungZf? "<li>Zugführer</li>":"");
        h += (ausbildungRangierer? "<li>Rangierer</li>":"");
        h += "</ul>";
    }

    // Sonstiges
    if (bemerkungen != "") {
        h += "<h3>Sonstiges</h3><p>";
        h += help.arg("Bemerkung").arg(bemerkungen);
        h += "</p>";
    }
    return h;
}

QString Person::getPersonaldatenFuerListeAlsCSV()
{
    return QString::number(nummer)
            +";"+nachname
            +";"+vorname
            +";"+geburtstag.toString("dd.MM.yyyy")
            +";"+eintritt.toString("dd.MM.yyyy")
            +";"+(aktiv?"Aktiv":"Passiv")
            +";"+austritt.toString("dd.MM.yyyy")

            +";"+(ausbildungTf ? "WAHR":"FALSCH")
            +";"+(ausbildungZf ? "WAHR":"FALSCH")
            +";"+(ausbildungRangierer ? "WAHR":"FALSCH")
            +";"+tauglichkeit.toString("dd.MM.yyyy")

            +";"+strasse
            +";"+plz
            +";"+ort
            +";"+mail
            +";"+(mailOK ? "WAHR" : "FALSCH")
            +";"+telefon
            +";"+(telefonOK ? "WAHR" : "FALSCH")

            +";"+QString::number(strecke)
            +";"+beruf
            +";"+bemerkungen.replace("\n","<br/>")
            +"\n";
}

int Person::getAdditional(Category cat)
{
    return additional.value(cat, 0);
}
void Person::setAdditional(Category cat, int value)
{
    additional.insert(cat, value);
    emit changed();
    valuesInvalid = true;
}

QString Person::getId() const
{
    return id;
}

QDate Person::getGeburtstag() const
{
    return geburtstag;
}
void Person::setGeburtstag(const QDate &value)
{
    geburtstag = value;
    emit changed();
}

bool Person::isMinderjaehrig()
{
    if (geburtstag.isNull()) return false;
    return geburtstag.addYears(18) > QDate::currentDate();
}

QDate Person::getEintritt() const
{
    return eintritt;
}
void Person::setEintritt(const QDate &value)
{
    eintritt = value;
    emit changed();
}

bool Person::getAktiv() const
{
    return aktiv;
}
void Person::setAktiv(bool value)
{
    aktiv = value;
    emit changed();
}

QDate Person::getAustritt() const
{
    return austritt;
}
void Person::setAustritt(const QDate &value)
{
    austritt = value;
    emit changed();
}

QDate Person::getTauglichkeit() const
{
    return tauglichkeit;
}
void Person::setTauglichkeit(const QDate &value)
{
    tauglichkeit = value;
    emit changed();
}
