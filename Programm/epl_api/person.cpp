#include "einstellungen.h"
#include "managerpersonal.h"
#include "person.h"
#include "aactivity.h"
#include "export.h"

#include <QJsonArray>
#include <QRandomGenerator>

const QString Person::FARBE_FEHLENDE_STUNDEN = "#ff9999";
const QString Person::FARBE_GENUG_STUNDEN = "#99ff99";
const QString Person::FARBE_STANDARD = "#ffffff";
const QString Person::KOPF_TABELLE_LISTE_CSV = "Nummer;Nachname;Vorname;Geburtsdatum;Geschlecht;Anrede;Beruf;"
                                               "Eintritt;Status;Austritt;Beiragsart;IBAN;Bank;Kontoinhaber;"
                                               "Beitrag;Nachzahlung;"
                                               "Tf;Zf;Rangierer;Tauglichkeit;BemerkungBetrieb;AusbildungSonst;"
                                               "Straße;PLZ;Ort;Strecke;Mail;Telefon;Telefon2;"
                                               "Zustimmung Mail;Zustimmung Telefon;"
                                               "Bemerkung\n";
const QString Person::FUSS_TABELLE_LISTE_HTML = "</tbody></table>";

const QStringList Person::ANZEIGE_PERSONALDATEN = {"Vorname", "Nachname", "Geburtsdatum", "Geschlecht", "Anrede", "Beruf",
                                                   "Nummer", "Eintritt", "Status", "Austritt", "Beitragsart", "IBAN", "Bank", "Kontoinhaber", "Beitrag", "Beitrag (Nachzahlung)",
                                                   "Straße", "PLZ", "Ort", "Strecke", "Mail", "Telefon", "Telefon2",
                                                   "Tf", "Zf", "Rangierer", "Tauglichkeit", "Bemerkung Betrieb.", "Sonst. Ausbildung",
                                                   "Mail Zustimmung", "Telefon Zustimmung",
                                                   "Bemerkung"};

QString Person::getKopfTabelleListeHtml(QSet<QString> data)
{
    QString kopf = "<h3>%1 – Stand %2</h3>"
                   "<table cellspacing='0' width='100%'><thead><tr>";
    if (data.contains("Nummer")
            || data.contains("Status")
            || data.contains("Eintritt")
            || data.contains("Austritt") || data.isEmpty())
        kopf += "<th>Mitgliedsdaten</th>";
    if (data.contains("Vorname")
            || data.contains("Nachname")
            || data.contains("Geburtsdatum")
            || data.contains("Anrede")
            || data.contains("Geschlecht")
            || data.contains("Beruf") || data.isEmpty())
        kopf += "<th>Persönliches</th>";
    if (data.contains("Beitragsart")
            || data.contains("IBAN")
            || data.contains("Bank")
            || data.contains("Beitrag")
            || data.contains("Beitrag (Nachzahlung)")
            || data.contains("Kontoinhaber") || data.isEmpty())
        kopf += "<th>Beitrag</th>";
    if (data.contains("Straße")
            || data.contains("PLZ")
            || data.contains("Ort")
            || data.contains("Strecke") || data.isEmpty())
        kopf += "<th>Anschrift</th>";
    if (data.contains("Mail")
            || data.contains("Telefon")
            || data.contains("Telefon2") || data.isEmpty())
        kopf += "<th>Kontakt</th>";
    if (data.contains("Tf")
            || data.contains("Zf")
            || data.contains("Rangierer")
            || data.contains("Tauglichkeit")
            || data.contains("Bemerkung Betrieb.")
            || data.contains("Sonst. Ausbildung") || data.isEmpty())
        kopf += "<th>Ausbildung</th>";
    if (data.contains("Mail Zustimmung")
            || data.contains("Telefon Zustimmung")
            || data.contains("Bemerkung") || data.isEmpty())
        kopf += "<th>Sonstiges</th>";
    return kopf + "</tr></thead><tbody>";
}

QString Person::toString(Person::Geschlecht g)
{
    switch (g) {
    case GeschlechtUnbekannt:
        return tr("unbekannt");
    case Juristisch:
        return tr("juristische Person");
    case Maennlich:
        return tr("männlich");
    case Weiblich:
        return tr("weiblich");
    case Divers:
        return tr("divers");
    }
}

QString Person::toString(Person::Beitragsart b)
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


Person::Person(QString name, ManagerPersonal *man) : QObject()
{
    name = ManagerPersonal::getGoodName(name);
    vorname = "";
    nachname = "";
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

QString Person::getRandomID()
{
    return QString::number(QDateTime::currentDateTime().toSecsSinceEpoch())+QString::number(QRandomGenerator::global()->bounded(1000000,9999999));
}

void Person::anfuegen(QString *zelle, QString appendix, QString seperator)
{
    if (appendix == "") return;
    if (!zelle->isEmpty())
        zelle->append(seperator);
    zelle->append(appendix);
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

QJsonObject Person::toJson() const
{
    QJsonObject o = personalToJson();
    // Zusätzliche Zeiten einfügen
    QJsonArray keys;
    QJsonArray values;
    for (auto it = additional.cbegin(); it != additional.cend(); ++it) {
        if (it.value() > 0) {
            keys.append(int(it.key()));
            values.append(it.value());
        }
    }
    o.insert("additionalKeys", keys);
    o.insert("additionalValues", values);
    return o;
}

QJsonObject Person::personalToJson() const
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

int Person::getZeiten(Category cat) const
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

bool Person::lessByNumber(const Person* const &lhs, const Person* const &rhs)
{
    return lhs->nummer < rhs->nummer;
}

bool Person::lessByName(const Person* const &lhs, const Person* const &rhs)
{
    if (lhs->getFullCanonicalName() < rhs->getFullCanonicalName())
        return true;
    if (lhs->getFullCanonicalName() > rhs->getFullCanonicalName())
        return false;
    if (lhs->getGeburtstag() < rhs->getGeburtstag())
        return true;
    if (lhs->getGeburtstag() > rhs->getGeburtstag())
        return false;
    return lessByNumber(lhs, rhs);
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

bool Person::isTauglich(Category cat, QDate datum) const
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

bool Person::isAusgetreten() const
{
    if (! austritt.isValid()) return false;
    return (austritt <= QDate::currentDate());
}

bool Person::pruefeFilter(Status filter)
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

Status Person::pruefeStunden() const
{
    if (isAusgetreten()) return Status::Ausgetreten;
    if (! getAktiv()) {
        if (getZeiten(Gesamt) > 0)
            return Status::PassivMit;
        else return Status::Passiv;
    }
    for (Category cat: ANZEIGEREIHENFOLGEGESAMT) {
        if (pruefeStunden(cat) == AktivOhne) {
            return Status::AktivOhne;
        }
    }
    return Status::AktivMit;
}

Status Person::pruefeStunden(Category cat) const
{
    if (isAusgetreten()) return Status::Ausgetreten;
    if (! getAktiv()) {
        if (getZeiten(cat) > 0)
            return Status::PassivMit;
        else return Status::Passiv;
    }

    if (getZeiten(cat) >= getMinimumStunden(cat))
        return Status::AktivMit;
    else return Status::AktivOhne;
}

int Person::getMinimumStunden(Category cat) const
{
    bool halb = (eintritt.year() == QDate::currentDate().year() && eintritt.month() >= 7);

    if (isMinderjaehrig()) return 0;
    if (isAusgetreten()) return 0;
    switch (cat) {
    case Tf:
        if (! isTauglich(Tf) ) return 0;
        return manager->getMinimumHours(cat) * (halb ? 0.5 : 1);
    case Zf:
        if (! isTauglich(Zf)) return 0;
        return manager->getMinimumHours(cat) * (halb ? 0.5 : 1);
    case Ausbildung:
        if ((! getAusbildungTf() && ! getAusbildungZf() && ! getAusbildungRangierer()) || !isTauglich()) return 0;
        return manager->getMinimumHours(cat) * (halb ? 0.5 : 1);
    default:
        return manager->getMinimumHours(cat) * (halb ? 0.5 : 1);
    }
}

void Person::berechne() const
{
    zeiten.clear();

    QDateTime vorherigeEnde = QDateTime(QDate(1900, 1, 1), QTime(0,0));
    for(Einsatz *e: getActivities()) {
            if (! e->getAnrechnen()) continue;

            // Einsatzstunden
            int duration = e->getDauer();
            switch (e->getKategorie()) {
            case Begleiter:
                zeiten.insert(Zub, zeiten.value(Zub)+duration);
                break;
            case Anzahl: break;
            case Kilometer: break;
            case Gesamt: break;
            default:
                zeiten.insert(e->getKategorie(), zeiten.value(e->getKategorie())+duration);
                break;
            }
            zeiten.insert(Anzahl, zeiten.value(Anzahl)+1);
            if (vorherigeEnde > e->getVon()) {
                // Einsaetze ueberlappen:
                // -> nur reduzierte Zeit anrechen, keine Strecke anrechnen
                zeiten.insert(Gesamt, zeiten.value(Gesamt)+(vorherigeEnde.secsTo(e->getBis()) / 60));
            } else if (vorherigeEnde == e->getVon()) {
                // Einsaetze gehen nahtlos ineinander ueber:
                // -> Keine Strecke aber gesamte Zeit anrechnen
                zeiten.insert(Gesamt, zeiten.value(Gesamt)+duration);
            } else {
                // Luecke zwischen den Einsaetzen:
                // -> Strecke und gesamte Zeit anrechnen
                zeiten.insert(Gesamt, zeiten.value(Gesamt)+duration);
                if (e->getKategorie() != Category::Buero)
                    zeiten.insert(Kilometer, zeiten.value(Kilometer)+2*strecke);
            }
            vorherigeEnde = e->getBis();
    }

    for (auto it = additional.cbegin(); it != additional.cend(); ++it) {
       zeiten.insert(it.key(), zeiten.value(it.key())+it.value());
       switch (it.key()) {
       case Anzahl: break;
       case Gesamt: break;
       case Kilometer: break;
       default:
           zeiten.insert(Gesamt, zeiten.value(Gesamt)+it.value());
       }
    }
    valuesInvalid = false;
}

void Person::addActivity(Einsatz *e)
{
    activities.append(e);
    Einsatz::sort(&activities);
    valuesInvalid = true;
}

bool Person::removeActivity(Einsatz *e)
{
    valuesInvalid = true;
    return activities.removeAll(e);
}

const QList<Einsatz*> Person::getActivities() const
{
    Einsatz::sort(&activities);
    return activities;
}

QString Person::getName() const
{
    if (vorname != "" && nachname != "") return vorname + " " + nachname;
    else if (vorname != "") return vorname;
    else return nachname;
}

QString Person::getFullCanonicalName() const
{
    if (vorname != "" && nachname != "") return nachname + ", " + vorname;
    else if (vorname != "") return vorname;
    else return nachname;
}

QString Person::getNameSortierung() const
{
    switch (Einstellungen::getReihenfolgeVorNach()) {
    case Einstellungen::VornameNachname:
        return getName();
    case Einstellungen::NachnameVorname:
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
    QString zelle = "<td style='background-color: %1'>%2</td>";
    QString farbe = "";
    switch (pruefeStunden()) {
    case AktivOhne: farbe = FARBE_FEHLENDE_STUNDEN; break;
    case PassivMit: farbe = FARBE_GENUG_STUNDEN; break;
    default:        farbe = FARBE_STANDARD;
    }
    QString html = zelle.arg(farbe, getName());

    zelle = "<td align='right' style='background-color: %1'>%2</td>";
    for(Category cat: ANZEIGEREIHENFOLGEGESAMT) {
        if (!liste.contains(cat)) continue;

        switch (pruefeStunden(cat)) {
        case AktivOhne: farbe = Person::FARBE_FEHLENDE_STUNDEN; break;
        case PassivMit: farbe = Person::FARBE_GENUG_STUNDEN; break;
        default:        farbe = Person::FARBE_STANDARD;
        }
        if (getZeiten(cat) > 0) {
            if (cat == Kilometer || cat == Anzahl) {
                html += zelle.arg(farbe).arg(getZeiten(cat));
            } else {
                html += zelle.arg(farbe, minutesToHourStringShort(getZeiten(cat)));
            }
        } else {
            html += zelle.arg(farbe, "");
        }
    }
    return "<tr>" + html + "</tr>";
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
            if (pruefeStunden(cat) != Status::AktivOhne) helpcurrent = help.arg("", "");
            else helpcurrent = help.arg("style=\"color: red;\"", " (mindestens "+minutesToHourString(getMinimumStunden(cat))+")");
            html += helpcurrent.arg(::toString(cat), minutesToHourString(getZeiten(cat)));
        }
    }
    html += "</ul><br/><ul>";
    if (pruefeStunden(Gesamt) != Status::AktivOhne) helpcurrent = help.arg("", "");
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
        for (Einsatz *e: getActivities()) {
                html += "<tr><td>"+QLocale().toString(e->getActivity()->getDatum(), "dd.MM.yyyy")+"<br/>"+e->getActivity()->getStringShort();
                if (!e->getActivity()->getStringShort().contains(e->getActivity()->getAnlass()) && e->getActivity()->getAnlass() != "")
                    html += "<br/>"+e->getActivity()->getAnlass();
                html +="</td><td>"
                     + e->getVon().toString("HH:mm")+"-"+e->getBis().toString("HH:mm")+"</td><td>"
                     + ::toString(e->getKategorie()) + "</td><td>"
                     + e->getBemerkung() + "</td></tr>";
        }
        html += "</tbody></table>";
    } else {
        html += QString("<p>%1 %2 hat an keinen eingetragenen Aktivitäten teilgenommen!</p>").arg(vorname, nachname);
    }
    help = "<li>%1: %2</li>";
    QString h2 = "";
    for (Category cat: ANZEIGEREIHENFOLGE) {
        if (getAdditional(cat) > 0)
            h2 += help.arg(::toString(cat), minutesToHourString(getAdditional(cat)));
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

bool Person::printZeiten(QPrinter *printer)
{
    return Export::druckeHtml(getZeitenFuerEinzelAlsHTML() + Export::zeitStempel(false), printer);
}

QString Person::getPersonaldatenFuerListeAlsHTML(QSet<QString> anzeige) const
{
    if (anzeige.isEmpty())
        anzeige = QSet<QString>(ANZEIGE_PERSONALDATEN.begin(), ANZEIGE_PERSONALDATEN.end());

    QString h = "";

    // Mitgliedschaft
    QString zelle = "";
    bool zelleNutzen = false;
    if (anzeige.contains("Nummer")) {
        zelleNutzen = true;
        anfuegen(&zelle, QString::number(nummer));
    }
    if (anzeige.contains("Status")) {
        zelleNutzen = true;
        if (zelle != "") zelle += "<br/>";
        if (isAusgetreten())
            zelle += "Ehemals: ";
        zelle += (aktiv ? "Aktiv":"Passiv");
    }
    if (anzeige.contains("Eintritt")) {
        zelleNutzen = true;
        anfuegen(&zelle, eintritt.toString("dd.MM.yyyy"));
        if (anzeige.contains("Austritt") && !austritt.isNull()) {
            zelle += austritt.toString("-dd.MM.yyyy");
        }
    } else if (anzeige.contains("Austritt")) {
        if (!austritt.isNull()) {
            zelleNutzen = true;
            anfuegen(&zelle, "Austritt zum: "+austritt.toString("dd.MM.yyyy"));
        }
    }
    if (zelleNutzen) {
        h += "<td>"+zelle+"</td>";
    }

    // Persoenliches
    zelle = "";
    zelleNutzen = false;
    if (anzeige.contains("Anrede")) {
        zelleNutzen = true;
        anfuegen(&zelle, anrede);
    }
    if (anzeige.contains("Vorname")) {
        zelleNutzen = true;
        if (zelle != "") zelle += "<br/>";
        if (anzeige.contains("Nachname")) {
            zelle += getName();
        } else {
            zelle += vorname;
        }
    } else if (anzeige.contains("Nachname")) {
        zelleNutzen = true;
        anfuegen(&zelle, nachname);
    }
    if (anzeige.contains("Geschlecht")) {
        zelleNutzen = true;
        anfuegen(&zelle, toString(geschlecht));
    }
    if (anzeige.contains("Geburtsdatum")) {
        zelleNutzen = true;
        anfuegen(&zelle, geburtstag.toString("*dd.MM.yyyy"));
    }
    if (anzeige.contains("Beruf")) {
        zelleNutzen = true;
        anfuegen(&zelle, beruf);
    }
    if (zelleNutzen) {
        h += "<td>"+zelle+"</td>";
    }



    // Mitgliedsbeitrag
    zelle = "";
    zelleNutzen = false;
    if (anzeige.contains("Beitragsart")) {
        zelleNutzen = true;
        if (beitragsart != Person::Beitragsart::BeitragUnbekannt) {
            anfuegen(&zelle, toString(beitragsart));
        }
    }
    if (anzeige.contains("IBAN")) {
        zelleNutzen = true;
        anfuegen(&zelle, iban);
    }
    if (anzeige.contains("Bank")) {
        zelleNutzen = true;
        anfuegen(&zelle, bank);
    }
    if (anzeige.contains("Kontoinhaber")) {
        zelleNutzen = true;
        anfuegen(&zelle, getKontoinhaber());
    }
    if (anzeige.contains("Beitrag")) {
        zelleNutzen = true;
        anfuegen(&zelle, QString("%1€").arg(getBeitrag()/100.f, 0, 'f', 2));
    }
    if (anzeige.contains("Beitrag (Nachzahlung)")) {
        zelleNutzen = true;
        anfuegen(&zelle, QString("(+%1€)").arg(getBeitragNachzahlung()/100.f, 0, 'f', 2), anzeige.contains("Beitrag") ? " ": "<br/>");
    }
    if (zelleNutzen) {
        h += "<td>"+zelle+"</td>";
    }


    // Anschrift
    zelle = "";
    zelleNutzen = false;
    if (anzeige.contains("Straße")) {
        zelleNutzen = true;
        anfuegen(&zelle, strasse);
    }
    if (anzeige.contains("PLZ")) {
        zelleNutzen = true;
        anfuegen(&zelle, plz);
        if (anzeige.contains("Ort")) {
            anfuegen(&zelle, ort, " ");
        }
    } else if (anzeige.contains("Ort")) {
        zelleNutzen = true;
        anfuegen(&zelle, ort);
    }
    if (anzeige.contains("Strecke")) {
        zelleNutzen = true;
        anfuegen(&zelle, (strecke != 0 ? QString::number(strecke)+"km" : ""));
    }
    if (zelleNutzen) {
        h += "<td>"+zelle+"</td>";
    }


    // Kontakt
    zelle = "";
    zelleNutzen = false;
    if (anzeige.contains("Mail")) {
        zelleNutzen = true;
        anfuegen(&zelle, mail);
    }
    if (anzeige.contains("Telefon")) {
        zelleNutzen = true;
        anfuegen(&zelle, telefon);
    }
    if (anzeige.contains("Telefon2")) {
        zelleNutzen = true;
        anfuegen(&zelle, telefon2);
    }
    if (zelleNutzen) {
        h += "<td>"+zelle+"</td>";
    }


    // Betriebsdienst
    zelle = "";
    zelleNutzen = false;
    if (anzeige.contains("Tf")) {
        zelleNutzen = true;
        anfuegen(&zelle, (ausbildungTf ? "Treibfahrzeugführer" : ""));
    }
    if (anzeige.contains("Zf")) {
        zelleNutzen = true;
        anfuegen(&zelle, (ausbildungZf ? "Zugführer" : ""));
    }
    if (anzeige.contains("Rangierer")) {
        zelleNutzen = true;
        anfuegen(&zelle, (ausbildungRangierer ? "Rangierer" : ""));
    }
    if (anzeige.contains("Tauglichkeit")) {
        zelleNutzen = true;
        anfuegen(&zelle, tauglichkeit.toString("dd.MM.yyyy"));
    }
    if (anzeige.contains("Bemerkung Betrieb.")) {
        zelleNutzen = true;
        anfuegen(&zelle, sonstigeBetrieblich);
    }
    if (anzeige.contains("Sonst. Ausbildung")) {
        zelleNutzen = true;
        anfuegen(&zelle, sonstigeAusbildung);
    }
    if (zelleNutzen) {
        h += "<td>"+zelle+"</td>";
    }


    // Sonstiges
    zelle = "";
    zelleNutzen = false;
    if (anzeige.contains("Mail Zustimmung")) {
        zelleNutzen = true;
        anfuegen(&zelle, (mailOK ? "" : "Sperrvermerk Mail"));
    }
    if (anzeige.contains("Mail Zustimmung")) {
        zelleNutzen = true;
        anfuegen(&zelle, (telefonOK ? "" : "Sperrvermerk Telefon"));
    }
    if (anzeige.contains("Bemerkung")) {
        zelleNutzen = true;
        anfuegen(&zelle, bemerkungen);
    }
    if (zelleNutzen) {
        h += "<td>"+zelle+"</td>";
    }

    return "<tr>"+h+"</tr>";
}

QString Person::getPersonaldatenFuerEinzelAlsHTML() const
{
    QString h = "<h2>"+vorname+" "+nachname+"</h2>";
    QString absch = "";

    QString help = "<li>%1:\t\t %2</li>";

    if (geburtstag.isValid())
        absch += help.arg("Geburtstag", geburtstag.toString("dd.MM.yyyy"));
    if (anrede != "")
        absch += help.arg("Anrede", anrede);
    absch += help.arg("Geschlecht", toString(geschlecht));
    if (beruf != "")
        absch += help.arg("Beruf", beruf);
    if (absch != "")
        h += "<h3>Persönliche Daten</h3><ul>"+ absch + "</ul>";


    // Mitgliedsdaten
    absch = "";
    absch += help.arg("Mitgliedsnummer").arg(nummer);
    absch += help.arg("Status", (isAusgetreten() ? "Ehemals %1" : "%1")).arg((aktiv ? "Aktiv":"Passiv"));
    absch += help.arg("Eintritt", eintritt.toString("dd.MM.yyyy"));
    if (isAusgetreten()) {
        absch += help.arg("Austritt", austritt.toString("dd.MM.yyyy"));
    } else {
        if (austritt.isValid())
            absch += help.arg("Austritt zum", austritt.toString("dd.MM.yyyy"));
    }
    if (getBeitrag() != 0) {
        absch += help.arg("Beitrag", "%L1 (%L2 €)").arg(toString(beitragsart)).arg(getBeitrag()/100.f, 0, 'f', 2);
    } else {
        absch += help.arg("Beitrag", toString(beitragsart));
    }
    if (getBeitragNachzahlung() != 0) {
        absch += help.arg("Aktuelle Nachzahlung", "%1 €").arg(getBeitragNachzahlung()/100.f, 0, 'f', 2);
    }
    if (beitragsart == Person::Beitragsart::FamilienBeitragNutzer) {
        absch += help.arg("Zahler", getKontoinhaber());
    } else {
        absch += help.arg("Konto", "%2 bei %3").arg(iban, bank);
        absch += help.arg("Kontoinhaber", getKontoinhaberFinal());
    }
    if (absch != "")
        h += "<h3>Mitgliedschaft</h3><ul>" + absch + "</ul>";


    // Anschrift
    absch = "";
    if (strasse != "")
        absch += help.arg("Straße", strasse);
    if (plz != "" || ort != "")
        absch += help.arg("PLZ und Ort", "%1 %2").arg(plz, ort);
    if (strecke != 0)
        absch +=  help.arg("Strecke", "%1km").arg(strecke);
//    if (absch != "")
//        h += "<h3>Anschrift</h3><ul>" + absch + "</ul>";

    // Kontakt
//    absch = "";
    if (mail != "")
        absch += help.arg("Mail", mail);
    if (telefon != "") {
        if (telefon2 != "")
            absch += help.arg("Telefon", "%2, %3").arg(telefon, telefon2);
        else
            absch += help.arg("Telefon", telefon);
    } else {
        if (telefon2 != "")
            absch += help.arg("Telefon", telefon2);
    }
    if (absch != "")
        h += "<h3>Kontaktdaten</h3><ul>" + absch + "</ul>";

    // Ausbildung
    absch = "";
    if (tauglichkeit.isValid())
        absch += help.arg("Tauglichkeit bis", tauglichkeit.toString("dd.MM.yyyy"));
    QStringList ausbildungen;
    if (ausbildungTf)
        ausbildungen.append("Triebfahrzeugführer");
    if (ausbildungZf)
        ausbildungen.append("Zugführer");
    if (ausbildungRangierer)
        ausbildungen.append("Rangierer");
    if (! ausbildungen.empty()) {
        absch += help.arg("Ausbildung zum", ausbildungen.join(", "));
    }
    if (sonstigeBetrieblich != "")
        absch += help.arg("Sonstige Betriebliche Bemerkungen", "<br/><i>" + sonstigeBetrieblich + "</i></li>");
    if (sonstigeAusbildung != "")
        absch += help.arg("Sonstige Ausbildungen", "<br/><i>" + sonstigeAusbildung + "</i></li>");
    if (absch != "")
        h += "<h3>Ausbildung</h3><ul>" + absch + "</ul>";

    // Datenschutz
    h += "<h3>Datenschutz</h3><ul>";
    h += help.arg("Sperrvermerk Mail", (mailOK ? "Nein" : "Ja"));
    h += help.arg("Sperrvermerk Telefon", (telefonOK ? "Nein" : "Ja"));
    h += "</ul>";

    // Sonstiges
    absch = "";
    if (bemerkungen != "")
        absch += "Bemerkung:<br/><i>"+ bemerkungen + "</i>";
    if (absch != "")
        h += "<h3>Sonstiges</h3><p>" + absch + "</p>";

    return h;
}

bool Person::printPersonaldaten(QPrinter *printer) const
{
    return Export::druckeHtml(getPersonaldatenFuerEinzelAlsHTML() + Export::zeitStempel(), printer);
}

QString Person::getPersonaldatenFuerListeAlsCSV() const
{
    return QString::number(nummer)
            +";"+nachname
            +";"+vorname
            +";"+geburtstag.toString("dd.MM.yyyy")
            +";"+toString(geschlecht)
            +";"+anrede
            +";"+QString(beruf).replace(";", ",")

            +";"+eintritt.toString("dd.MM.yyyy")
            +";"+(aktiv?"Aktiv":"Passiv")
            +";"+austritt.toString("dd.MM.yyyy")
            +";"+toString(beitragsart)
            +";"+iban
            +";"+bank
            +";"+getKontoinhaberFinal()
            +";"+QString::number(getBeitrag()/100.f, 'f', 2).replace(".", ",")
            +";"+QString::number(getBeitragNachzahlung()/100.f, 'f', 2).replace(".", ",")

            +";"+(ausbildungTf ? "WAHR":"FALSCH")
            +";"+(ausbildungZf ? "WAHR":"FALSCH")
            +";"+(ausbildungRangierer ? "WAHR":"FALSCH")
            +";"+tauglichkeit.toString("dd.MM.yyyy")
            +";"+QString(sonstigeBetrieblich).replace("\n","<br/>").replace(";", ",")
            +";"+QString(sonstigeAusbildung).replace("\n","<br/>").replace(";", ",")

            +";"+strasse
            +";"+plz
            +";"+ort
            +";"+QString::number(strecke)
            +";"+mail
            +";"+telefon
            +";"+telefon2

            +";"+(mailOK ? "WAHR" : "FALSCH")
            +";"+(telefonOK ? "WAHR" : "FALSCH")

            +";"+QString(bemerkungen).replace("\n","<br/>").replace(";", ",")
            +"\n";
}

int Person::getAdditional(Category cat) const
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

bool Person::isMinderjaehrig() const
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
int Person::getBeitrag() const
{
    if (eintritt.year() == QDate::currentDate().year() && eintritt.month() >= 7) {
        return manager->getBeitrag(beitragsart) / 2;
    }
    return manager->getBeitrag(beitragsart);
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

QString Person::getKontoinhaberFinal() const
{
    QString kI = getKontoinhaber();
    if (kI != "")
        return kI;
    return getName();
}

QString Person::getKontoinhaber() const
{
    Person *pers = getKontoinhaberPerson();
    if (pers != nullptr)
        return pers->getName();
    return kontoinhaber;
}
Person* Person::getKontoinhaberPerson() const
{
    return manager->getPersonFromID(kontoinhaber);
}
void Person::setKontoinhaber(const QString &value)
{
    kontoinhaber = value;
    if (beitragsart == Person::Beitragsart::FamilienBeitragNutzer) {
        Person *zahler = manager->getPerson(value);
        if (zahler != nullptr) {
            kontoinhaber = zahler->getId();
        }
    }
    emit changed();
}

int Person::getBeitragRegulaerIndividuell() const
{
    if (beitragsart != Beitragsart::FamilienBeitragNutzer && beitragsart != Beitragsart::FamilienBeitragZahler)
        return getBeitrag();

    Person const *zahler = manager->getPersonFromID(kontoinhaber);
    if (beitragsart == Beitragsart::FamilienBeitragZahler)
        zahler = this;
    if (zahler == nullptr)
        return 0;

    int count = 1;
    int betrag = zahler->getBeitrag();
    for(Person *pers: manager->getPersonen(Status::AlleMitglieder)) {
        if (pers->getBeitragsart() != Person::Beitragsart::FamilienBeitragNutzer)
            continue;

        if (zahler != manager->getPersonFromID(pers->kontoinhaber))
            continue;

        if (pers->getMinimumStunden(Gesamt) != 0) {
            count ++;
            betrag += pers->getBeitrag();
        }
    }
    return betrag / count;
}

int Person::getBeitragNachzahlung() const
{
    if (! getAktiv() || isAusgetreten())
        return 0;
    if (beitragsart == Beitragsart::Beitragsfrei)
        return 0;
    if (Status::AktivMit == pruefeStunden(Gesamt))
        return 0;

    double prozent = 1.f - getZeiten(Gesamt) / (double)getMinimumStunden(Gesamt);
    int satz = manager->getBeitrag(Beitragsart::FoerderBeitrag);
//    if (eintritt.year() == QDate::currentDate().year() && eintritt.month() >= 7) {
//        satz = satz / 2;
//    }
    satz = satz - getBeitragRegulaerIndividuell();
    return (satz * prozent/100)*100;
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
