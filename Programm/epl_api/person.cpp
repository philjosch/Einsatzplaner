#include "einstellungen.h"
#include "managerpersonal.h"
#include "person.h"

#include <QJsonArray>
#include <QRandomGenerator>
#include <algorithm>
#include <cmath>

const QString Person::FARBE_FEHLENDE_STUNDEN = "#ff9999";
const QString Person::FARBE_GENUG_STUNDEN = "#99ff99";
const QString Person::FARBE_STANDARD = "#ffffff";
const QString Person::KOPF_TABELLE_LISTE_CSV = "Nummer;Nachname;Vorname;Geburtsdatum;Geschlecht;Anrede;Beruf;"
                                               "Eintritt;Status;Austritt;Beiragsart;IBAN;Bank;Kontoinhaber;"
                                               "Tf;Zf;Rangierer;Tauglichkeit;BemerkungBetrieb;AusbildungSonst;"
                                               "Straße;PLZ;Ort;Strecke;Mail;Telefon;Telefon2;"
                                               "Zustimmung Mail;Zustimmung Telefon;"
                                               "Bemerkung\n";
const QString Person::KOPF_TABELLE_LISTE_HTML = "<h3>%1 – Stand %2</h3>"
                                                   "<table cellspacing='0' width='100%'><thead><tr>"
                                                   "<th>Name<br/>Mitgliedsnummer<br/>Status</th>"
                                                   "<th>Geburtsdatum<br/>Eintritt<br/>Beruf</th>"
                                                   "<th>Anschrift</th>"
                                                   "<th>E-Mail<br/>Telefon</th>"
                                                   "<th>Betriebsdienst</th>"
                                                   "<th>Sonstiges</th>"
                                                   "</thead><tbody>";
const QString Person::FUSS_TABELLE_LISTE_HTML = "</tbody></table>";

QString Person::getStringVonGeschlecht(Person::Geschlecht g)
{
    switch (g) {
    case GeschlechtUnbekannt:
        return tr("Unbekannt");
    case Juristisch:
        return tr("Juristische Person");
    case Maennlich:
        return tr("Männlich");
    case Weiblich:
        return tr("Weiblich");
    case Divers:
        return tr("Divers");
    }
}

QString Person::getStringVonBeitragsart(Person::Beitragsart b)
{
    switch (b) {
    case BeitragUnbekannt:
        return tr("Unbekannt");
    case Beitragsfrei:
        return tr("beitragsbefreit");
    case AktivenBeitrag:
        return tr("Beitrag Aktive");
    case FoerderBeitrag:
        return tr("Förderbeitrag");
    case FamilienBeitragZahler:
        return tr("Familienbeitrag (Zahler)");
    case FamilienBeitragNutzer:
        return tr("Familienbeitrag (Nutzer)");
    case Ermaessigt:
        return tr("Ermäßigt");
    }
}

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
    activities = QList<Einsatz*>();

    valuesInvalid = true;
    manager = man;

    if (o.contains("id")) {
        id = o.value("id").toString();
    } else {
        id = getRandomID();
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
    anrede = o.value("anrede").toString();
    geschlecht = static_cast<Geschlecht>(o.value("geschlecht").toInt());
    beruf= o.value("beruf").toString();

    eintritt = QDate::fromString(o.value("eintritt").toString(), "yyyy-MM-dd");
    aktiv = o.value("aktiv").toBool(true);
    austritt = QDate::fromString(o.value("austritt").toString(), "yyyy-MM-dd");
    beitragsart = static_cast<Beitragsart>(o.value("beitragsart").toInt());
    iban = o.value("iban").toString();
    bank = o.value("bank").toString();
    kontoinhaber = o.value("kontoinhaber").toString();

    ausbildungRangierer = o.value("ausRang").toBool();
    ausbildungZf = o.value("ausZf").toBool();
    ausbildungTf = o.value("ausTf").toBool();
    tauglichkeit = QDate::fromString(o.value("tauglichkeit").toString(), "yyyy-MM-dd");
    sonstigeBetrieblich = o.value("sonstBetrieb").toString();
    sonstigeAusbildung = o.value("sonstigeAusb").toString();

    strasse = o.value("strasse").toString();
    plz = o.value("plz").toString();
    ort = o.value("ort").toString();
    strecke = o.value("strecke").toInt();
    mail = o.value("mail").toString();
    telefon = o.value("tel").toString();
    telefon2 = o.value("tel2").toString();

    mailOK = o.value("mailOK").toBool(true);
    telefonOK = o.value("telOK").toBool(true);

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

void Person::personConstructor(QString vn, QString nn, ManagerPersonal *man)
{
    additional = QMap<Category, int>();
    zeiten = QMap<Category, int>();
    activities = QList<Einsatz*>();

    valuesInvalid = true;
    manager = man;

    id = getRandomID();

    if (manager == nullptr)
        nummer = 1;
    else
        nummer = manager->getNextNummer();
    vorname = vn;
    nachname = nn;
    geburtstag = QDate();
    anrede = "";
    geschlecht = Geschlecht::GeschlechtUnbekannt;
    beruf = "";

    eintritt = QDate::currentDate();
    aktiv = true;
    austritt = QDate();
    beitragsart = Beitragsart::AktivenBeitrag;
    iban = "";
    bank = "";
    kontoinhaber = "";

    ausbildungTf = false;
    ausbildungZf = false;
    ausbildungRangierer = false;
    tauglichkeit = QDate();
    sonstigeBetrieblich = "";
    sonstigeAusbildung = "";

    strasse = "";
    plz = "";
    ort = "";
    strecke = 0;
    mail = "";
    telefon = "";
    telefon2 = "";

    mailOK = true;
    telefonOK = true;
    // Sonstiges
    bemerkungen = "";
}

QString Person::getRandomID()
{
    return QString::number(QDateTime::currentDateTime().toSecsSinceEpoch())+QString::number(QRandomGenerator::global()->bounded(1000000,9999999));
}

ManagerPersonal *Person::getManager() const
{
    return manager;
}

QString Person::getAnrede() const
{
    return anrede;
}
void Person::setAnrede(const QString &value)
{
    anrede = value;
    emit changed();
}

Person::Geschlecht Person::getGeschlecht() const
{
    return geschlecht;
}
void Person::setGeschlecht(const Person::Geschlecht &value)
{
    geschlecht = value;
    emit changed();
}

QString Person::getSonstigeBetrieblich() const
{
    return sonstigeBetrieblich;
}
void Person::setSonstigeBetrieblich(const QString &value)
{
    sonstigeBetrieblich = value;
    emit changed();
}

QString Person::getSonstigeAusbildung() const
{
    return sonstigeAusbildung;
}
void Person::setSonstigeAusbildung(const QString &value)
{
    sonstigeAusbildung = value;
    emit changed();
}

QString Person::getTelefon2() const
{
    return telefon2;
}
void Person::setTelefon2(const QString &value)
{
    telefon2 = value;
    emit changed();
}

QJsonObject Person::toJson()
{
    QJsonObject o = personalToJson();
    // Zusätzliche Zeiten einfügen
    QJsonArray keys;
    QJsonArray values;
    for (Category cat: additional.keys()) {
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
    o.insert("anrede", anrede);
    o.insert("geschlecht", geschlecht);
    o.insert("beruf", beruf);

    o.insert("eintritt", eintritt.toString("yyyy-MM-dd"));
    o.insert("aktiv", aktiv);
    o.insert("austritt", austritt.toString("yyyy-MM-dd"));
    o.insert("beitragsart", beitragsart);
    o.insert("iban", iban);
    o.insert("bank", bank);
    o.insert("kontoinhaber", kontoinhaber);

    o.insert("ausTf", ausbildungTf);
    o.insert("ausZf", ausbildungZf);
    o.insert("ausRang", ausbildungRangierer);
    o.insert("tauglichkeit", tauglichkeit.toString("yyyy-MM-dd"));
    o.insert("sonstBetrieb", sonstigeBetrieblich);
    o.insert("sonstigeAusb", sonstigeAusbildung);

    o.insert("strasse", strasse);
    o.insert("plz", plz);
    o.insert("ort", ort);
    o.insert("strecke", strecke);
    o.insert("mail", mail);
    o.insert("tel", telefon);
    o.insert("tel2", telefon2);

    o.insert("mailOK", mailOK);
    o.insert("telOK", telefonOK);

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

int Person::setAutoNummer()
{
    nummer = manager->getNextNummer();
    emit changed();
    return nummer;
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
    for (Category cat: ANZEIGEREIHENFOLGEGESAMT) {
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
    QSet<AActivity*> actis;
    for(Einsatz *e: activities) {
        actis.insert(e->activity);
    }

    for(Einsatz ee: getActivities()) {
            Einsatz *e = &(ee);
            if (! e->anrechnen) continue;

            // Einsatzstunden
            QTime start = e->beginn;
            QTime ende = e->ende;
            int duration = (start.msecsTo(ende) / 60000); // in Minuten
            switch (e->kategorie) {
            case Begleiter:
                zeiten.insert(Zub, zeiten.value(Zub)+duration);
                break;
            case Anzahl: break;
            case Kilometer: break;
            case Gesamt: break;
            default:
                zeiten.insert(e->kategorie, zeiten.value(e->kategorie)+duration);
                break;
            }
            zeiten.insert(Anzahl, zeiten.value(Anzahl)+1);
            zeiten.insert(Gesamt, zeiten.value(Gesamt)+duration);
            if (e->kategorie != Category::Buero)
                zeiten.insert(Kilometer, zeiten.value(Kilometer)+2*strecke);
    }
    for (Category cat: additional.keys()) {
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

bool Person::addActivity(Einsatz *e)
{
    activities.append(e);
    return true;
}

bool Person::removeActivity(Einsatz *e)
{
    return activities.removeAll(e);
}

QList<Einsatz> Person::getActivities()
{
    QSet<AActivity*> actis;
    for(Einsatz *e: activities) {
        actis.insert(e->activity);
    }

    QList<Einsatz> liste;
    for(AActivity *activity: actis) {
        liste.append(activity->getIndividual(this));
    }
    Einsatz::sort(&liste);

    return liste;
}

QString Person::getName() const
{
    QString nameKomplett;
    if (vorname != "" && nachname != "") return vorname + " " + nachname;
    else if (vorname != "") return vorname;
    else return nachname;
}

QString Person::getNameSortierung() const
{
    if (Einstellungen::getReihenfolgeVorNach()) {
        return getName();
    } else {
        if (vorname != "" && nachname != "") return nachname + ", " + vorname;
        else if (vorname != "") return vorname;
        else return nachname;
    }
}

QString Person::getVorname() const
{
    return vorname;
}
bool Person::setVorname(const QString &value)
{
    if (manager->personExists(value, nachname)) {
        return false;
    }
    QString old = getName();
    vorname = value;
    while (vorname.endsWith(" ")) vorname.chop(1);
    while (vorname.startsWith(" ")) vorname = vorname.remove(0, 1);
    emit nameChanged(this, old);
    emit changed();
    return true;
}

QString Person::getNachname() const
{
    return nachname;
}
bool Person::setNachname(const QString &value)
{
    if (manager->personExists(vorname, value)) {
        return false;
    }
    QString old = getName();
    nachname = value;
    while (nachname.endsWith(" ")) nachname.chop(1);
    while (nachname.startsWith(" ")) nachname = nachname.remove(0, 1);
    emit nameChanged(this, old);
    emit changed();
    return true;
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

    for(Category cat: ANZEIGEREIHENFOLGEGESAMT) {
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
    for (Category cat: ANZEIGEREIHENFOLGE) {
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
        for (Einsatz e: getActivities()) {
                html += "<tr><td>"+e.activity->getDatum().toString("dd.MM.yyyy")+"<br/>"+e.activity->getKurzbeschreibung();
                if (e.activity->getAnlass() != e.activity->getKurzbeschreibung() && e.activity->getAnlass() != "")
                    html += "<br/>"+e.activity->getAnlass();
                html +="</td><td>"
                     + e.beginn.toString("HH:mm")+"-"+e.ende.toString("HH:mm")+"</td><td>"
                     + getLocalizedStringFromCategory(e.kategorie) + "</td><td>"
                     + e.bemerkung + "</td></tr>";
        }
        html += "</tbody></table>";
    } else {
        html += QString("<p>%1 %2 hat an keinen eingetragenen Aktivitäten teilgenommen!</p>").arg(vorname, nachname);
    }
    help = "<li>%1: %2</li>";
    QString h2 = "";
    for (Category cat: ANZEIGEREIHENFOLGE) {
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
    // TODO: Muss ueberarbeitet werden, wenn die Exportfunktion ueberarbeitet wird!!!!
    // Ebenso muessen die neuen Werte it ausgegeben werden!!!
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
    QString absch = "";

    QString help = "<li>%1:\t\t %2</li>";

    if (geburtstag.isValid())
        absch += help.arg("Geburtstag").arg(geburtstag.toString("dd.MM.yyyy"));
    if (anrede != "")
        absch += help.arg("Anrede").arg(anrede);
    absch += help.arg("Geschlecht").arg(getStringVonGeschlecht(geschlecht));
    if (beruf != "")
        absch += help.arg("Beruf").arg(beruf);
    if (absch != "")
        h += "<h3>Persönliche Daten</h3><ul>"+ absch + "</ul>";


    // Mitgliedsdaten
    absch = "";
    absch += help.arg("Mitgliedsnummer").arg(nummer);
    absch += help.arg("Status").arg( isAusgetreten() ? "Ehemals %1" : "%1")
            .arg(aktiv ? "Aktiv":"Passiv");
    absch += help.arg("Eintritt").arg(eintritt.toString("dd.MM.yyyy"));
    if (isAusgetreten()) {
        absch += help.arg("Austritt").arg(austritt.toString("dd.MM.yyyy"));
    } else {
        if (austritt.isValid())
            absch += help.arg("Austritt zum").arg(austritt.toString("dd.MM.yyyy"));
    }
    absch += help.arg("Beitragsart").arg(getStringVonBeitragsart(beitragsart));
    if (beitragsart == Person::Beitragsart::FamilienBeitragNutzer) {
        absch += help.arg("Zahler").arg(getKontoinhaberText());
    } else {
        absch += help.arg("Konto").arg("%2 bei %3").arg(iban).arg(bank);
        absch += help.arg("Kontoinhaber").arg(kontoinhaber);
    }
    if (absch != "")
        h += "<h3>Mitgliedschaft</h3><ul>" + absch + "</ul>";


    // Anschrift
    absch = "";
    if (strasse != "")
        absch += help.arg("Straße").arg(strasse);
    if (plz != "" || ort != "")
        absch += help.arg("PLZ und Ort").arg("%1 %2").arg(plz).arg(ort);
    if (strecke != 0)
        absch +=  help.arg("Strecke").arg("%1km").arg(strecke);
//    if (absch != "")
//        h += "<h3>Anschrift</h3><ul>" + absch + "</ul>";

    // Kontakt
//    absch = "";
    if (mail != "")
        absch += help.arg("Mail").arg(mail);
    if (telefon != "") {
        if (telefon2 != "")
            absch += help.arg("Telefon").arg("%2, %3").arg(telefon).arg(telefon2);
        else
            absch += help.arg("Telefon").arg(telefon);
    } else {
        if (telefon2 != "")
            absch += help.arg("Telefon").arg(telefon2);
    }
    if (absch != "")
        h += "<h3>Kontaktdaten</h3><ul>" + absch + "</ul>";

    // Ausbildung
    absch = "";
    if (tauglichkeit.isValid())
        absch += help.arg("Tauglichkeit bis").arg(tauglichkeit.toString("dd.MM.yyyy"));
    if (ausbildungTf || ausbildungZf || ausbildungRangierer) {
        bool komma = false;
        absch += help.arg("Ausbildung zum");
        if (ausbildungTf) {
            komma = true;
            absch = absch.arg("Triebfahrzeugführer%1");
        }
        if (ausbildungZf) {
            if (komma) absch = absch.arg(", %1");
            komma = true;
            absch = absch.arg("Zugführer%1");
        }
        if (ausbildungRangierer) {
            if (komma) absch = absch.arg(" und %1");
            absch = absch.arg("Rangierer");
        }
    }
    if (sonstigeBetrieblich != "")
        absch += "<li>Sonstige Betriebliche Bemerkungen:<br/><i>" + sonstigeBetrieblich + "</i></li>";
    if (sonstigeAusbildung != "")
        absch += "<li>Sonstige Ausbildungen:<br/><i>" + sonstigeAusbildung + "</i></li>";
    if (absch != "")
        h += "<h3>Ausbildung</h3><ul>" + absch + "</ul>";

    // Datenschutz
    h += "<h3>Datenschutz</h3><ul>";
    h += help.arg("Sperrvermerk Mail").arg(mailOK ? "Nein" : "Ja");
    h += help.arg("Sperrvermerk Telefon").arg(telefonOK ? "Nein" : "Ja");
    h += "</ul>";

    // Sonstiges
    absch = "";
    if (bemerkungen != "")
        absch += "Bemerkung:<br/><i>"+ bemerkungen + "</i>";
    if (absch != "")
        h += "<h3>Sonstiges</h3><p>" + absch + "</p>";

    return h;
}

QString Person::getPersonaldatenFuerListeAlsCSV()
{
    return QString::number(nummer)
            +";"+nachname
            +";"+vorname
            +";"+geburtstag.toString("dd.MM.yyyy")
            +";"+getStringVonGeschlecht(geschlecht)
            +";"+anrede
            +";"+beruf

            +";"+eintritt.toString("dd.MM.yyyy")
            +";"+(aktiv?"Aktiv":"Passiv")
            +";"+austritt.toString("dd.MM.yyyy")
            +";"+getStringVonBeitragsart(beitragsart)
            +";"+iban
            +";"+bank
            +";"+getKontoinhaberText()
            +";"+sonstigeBetrieblich.replace("\n","<br/>")
            +";"+sonstigeAusbildung.replace("\n","<br/>")

            +";"+(ausbildungTf ? "WAHR":"FALSCH")
            +";"+(ausbildungZf ? "WAHR":"FALSCH")
            +";"+(ausbildungRangierer ? "WAHR":"FALSCH")
            +";"+tauglichkeit.toString("dd.MM.yyyy")


            +";"+strasse
            +";"+plz
            +";"+ort
            +";"+QString::number(strecke)
            +";"+mail
            +";"+telefon
            +";"+telefon2

            +";"+(mailOK ? "WAHR" : "FALSCH")
            +";"+(telefonOK ? "WAHR" : "FALSCH")

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

Person::Beitragsart Person::getBeitragsart() const
{
    return beitragsart;
}
void Person::setBeitragsart(const Person::Beitragsart &value)
{
    beitragsart = value;
    emit changed();
}

QString Person::getIban() const
{
    return iban;
}
void Person::setIban(const QString &value)
{
    iban = value;
    emit changed();
}

QString Person::getBank() const
{
    return bank;
}
void Person::setBank(const QString &value)
{
    bank = value;
    emit changed();
}

QString Person::getKontoinhaber() const
{
    return kontoinhaber;
}
QString Person::getKontoinhaberText()
{
    if (beitragsart == Person::Beitragsart::FamilienBeitragNutzer) {
        Person *pers = manager->getPersonFromID(kontoinhaber);
        if (pers != nullptr)
            return pers->getName();
    }
    return kontoinhaber;
}
void Person::setKontoinhaber(const QString &value)
{
    kontoinhaber = value;
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
