#include "export.h"
#include "fileio.h"
#include "managerpersonal.h"
#include "person.h"

#include <QJsonArray>

const QMap<Category, int> ManagerPersonal::MINIMUM_HOURS_DEFAULT {{Category::Tf, 100*60}, {Gesamt, 10*60}};
const QMap<Person::Beitragsart, int> ManagerPersonal::BEITRAEGE_DEFAULT {{Person::BeitragUnbekannt, 0},
                                                                         {Person::Beitragsfrei, 0},
                                                                         {Person::AktivenBeitrag, 3000},
                                                                         {Person::FoerderBeitrag, 6000},
                                                                         {Person::FamilienBeitragZahler, 3600},
                                                                         {Person::FamilienBeitragNutzer, 0},
                                                                         {Person::Ermaessigt, 1500},
                                                                        };

ManagerPersonal::ManagerPersonal()
{
    personen = QList<Person*>();
    minimumHours = MINIMUM_HOURS_DEFAULT;
    beitraege = BEITRAEGE_DEFAULT;
}

ManagerPersonal::ManagerPersonal(QJsonObject o)
{
    personen = QList<Person*>();
    minimumHours = QMap<Category, int>();
    beitraege = QMap<Person::Beitragsart, int>();

    QJsonArray a = o.value("personen").toArray();
    for(int i = 0; i < a.size(); i++) {
        Person *neu = new Person(a.at(i).toObject(), this);
        personen.append(neu);
        connect(neu, &Person::changed, this, [=]() { emit personChanged(neu); });
        connect(neu, &Person::changed, this, [=]() { emit changed();} );
    }
    if (o.contains("minimumKeys") && o.contains("minimumValues")) {
        QJsonArray keys = o.value("minimumKeys").toArray();
        QJsonArray values = o.value("minimumValues").toArray();
        for (int i = 0; i < keys.size() && i < values.size() ; i++) {
            minimumHours.insert(Category(keys.at(i).toInt()), values.at(i).toInt());
        }
    } else {
        minimumHours.insert(Gesamt, int(o.value("minimumTotal").toDouble(0)*60));
        if (o.contains("minimumHours")) {
            QJsonObject o2 = o.value("minimumHours").toObject();
            for(auto it = o2.constBegin(); it != o2.constEnd(); ++it) {
                Category catt = getCategoryFromLocalizedString(it.key());
                minimumHours.insert(catt, int(it.value().toDouble(0)*60));
            }
        }
    }
    // Beitraege
    if (o.contains("beitraegeKeys") && o.contains("beitraegeValues")) {
        QJsonArray keysBeitraege = o.value("beitraegeKeys").toArray();
        QJsonArray valuesBeitraege = o.value("beitraegeValues").toArray();
        for (int i = 0; i < keysBeitraege.size() && i < valuesBeitraege.size() ; i++) {
            beitraege.insert(Person::Beitragsart(keysBeitraege.at(i).toInt()), valuesBeitraege.at(i).toInt());
        }
    } else {
        beitraege = BEITRAEGE_DEFAULT;
    }
}

ManagerPersonal::~ManagerPersonal()
{

}

QJsonObject ManagerPersonal::toJson() const
{
    // Personen
    QJsonArray array;
    for(Person *p: getPersonenSortiertNachNummer()) {
        array.append(p->toJson());
    }
    // Mindesstunden
    QJsonArray keysMinimum;
    QJsonArray valuesMinimum;
    for(auto it = minimumHours.cbegin(); it != minimumHours.cend(); ++it) {
        if (it.key() > 0) {
            keysMinimum.append(int(it.key()));
            valuesMinimum.append(it.value());
        }
    }
    // Beitraege
    QJsonArray keysBeitraege;
    QJsonArray valuesBeitraege;
    for (auto it = beitraege.cbegin(); it != beitraege.cend(); ++it) {
        if (it.value() != 0) {
            keysBeitraege.append(int(it.key()));
            valuesBeitraege.append(it.value());
        }
    }

    QJsonObject o;
    o.insert("personen", array);
    o.insert("minimumKeys", keysMinimum);
    o.insert("minimumValues", valuesMinimum);
    o.insert("beitraegeKeys", keysBeitraege);
    o.insert("beitraegeValues", valuesBeitraege);
    return o;
}

QJsonObject ManagerPersonal::personalToJson() const
{
    // Personen
    QJsonArray array;
    for(Person *p: personen) {
        array.append(p->personalToJson());
    }
    // Mindesstunden
    QJsonArray keys;
    QJsonArray values;
    for (auto it = minimumHours.cbegin(); it != minimumHours.cend(); ++it) {
        if (it.value() > 0) {
            keys.append(int(it.key()));
            values.append(it.value());
        }
    }
    // Beitraege
    QJsonArray keysBeitraege;
    QJsonArray valuesBeitraege;
    for(auto it = beitraege.cbegin(); it != beitraege.cend(); ++it) {
        if (it.value() != 0) {
            keysBeitraege.append(int(it.key()));
            valuesBeitraege.append(it.value());
        }
    }

    QJsonObject o;
    o.insert("personen", array);
    o.insert("minimumKeys", keys);
    o.insert("minimumValues", values);
    o.insert("beitraegeKeys", keysBeitraege);
    o.insert("beitraegeValues", valuesBeitraege);
    return o;
}

Person *ManagerPersonal::getPersonFromID(QString id) const
{
    for(Person *p: personen) {
        if (p->getId() == id)
            return p;
    }
    return nullptr;
}

Person *ManagerPersonal::getPerson(QString name) const
{
    name = getGoodName(name);
    for(Person *p: personen) {
        if (p->getName() == name)
            return p;
    }
    return nullptr;
}

bool ManagerPersonal::personExists(QString name) const
{
    return getPerson(name) != nullptr;
}

bool ManagerPersonal::personExists(QString vorname, QString nachname) const
{
    while (vorname.endsWith(" ")) vorname.chop(1);
//    while (vorname.startsWith(" ")) vorname = vorname.remove(0, 1);
//    while (nachname.endsWith(" ")) nachname.chop(1);
    while (nachname.startsWith(" ")) nachname = nachname.remove(0, 1);
//    QString nameKomplett;
//    if (vorname != "") nameKomplett = vorname + " " + nachname;
//    else nameKomplett = nachname;
    return personExists(vorname + " " + nachname);
}

Person *ManagerPersonal::newPerson()
{
    QString name = tr("Unbekannt Unbenannt");
    if (personExists(name)) return nullptr;
    Person *neu = new Person(name, this);
    personen.append(neu);
    connect(neu, &Person::changed, this, [=]() { emit personChanged(neu); });
    connect(neu, &Person::changed, this, [=]() { emit changed();});
    emit changed();
    return neu;
}

bool ManagerPersonal::removePerson(Person *p)
{
    if (personen.contains(p)) {
        personen.removeAll(p);
        emit changed();
        return true;
    }
    return false;
}

void ManagerPersonal::setMinimumHours(Category cat, int amount)
{
    minimumHours.insert(cat, amount);
    emit changed();
}
int ManagerPersonal::getMinimumHours(Category cat) const
{
    return minimumHours.value(cat, 0);
}

int ManagerPersonal::getMinimumHoursDefault(Category cat)
{
    return MINIMUM_HOURS_DEFAULT.value(cat, 0);
}

void ManagerPersonal::setBeitrag(Person::Beitragsart art, int beitrag)
{
    beitraege.insert(art, beitrag);
    emit changed();
}
int ManagerPersonal::getBeitrag(Person::Beitragsart art) const
{
    return beitraege.value(art, 0);
}

QList<Person *> ManagerPersonal::getPersonenSortiertNachNummer() const
{
    QList<Person *> l;
    int i = 0;
    for (Person *p: personen) {
        l.append(p);
        i = l.size()-1;
        while(i > 0 && l.at(i-1)->getNummer() > l.at(i)->getNummer()) {
            l.swapItemsAt(i, i-1);
            i--;
        }
    }
    return l;
}

QString ManagerPersonal::getGoodName(QString name)
{
    while (name.endsWith(" ")) name.chop(1);
    while (name.startsWith(" ")) name = name.remove(0, 1);
    if (name.contains(QRegExp("\\s*,\\s*"))) {
        QStringList liste = name.split(QRegExp("\\s*,\\s*"));
        name = liste.at(1) + " " + liste.at(0);
    }
    return name;
}

int ManagerPersonal::getNextNummer() const
{
    int max = 0;
    for (Person *p: personen) {
        if (p->getNummer() > max) max = p->getNummer();
    }
    return max+1;
}

bool ManagerPersonal::checkNummer(int neu) const
{
    for(Person *p: personen) {
        if (p->getNummer() == neu) return false;
    }
    return true;
}

bool ManagerPersonal::printZeitenEinzel(QList<Person *> liste, Status filter, QPrinter *printer) const
{
    QString a = "";
    // Seite fuer jede Person einfuegen
    QMap<Category, int> sum;
    for(Person *p: liste) {
        a += p->getZeitenFuerEinzelAlsHTML();
        a += Export::zeitStempel(p != liste.last());
        for (Category cat: ANZEIGEREIHENFOLGEGESAMT) {
            sum.insert(cat, sum.value(cat,0)+p->getZeiten(cat));
        }
    }

    QString help = "<li>%1: %2</li>";
    QString titelSeite = QString("<h1>Einsatzzeiten: %1</h1><h2>Geleistete Stunden</h2><ul>").arg(toString(filter));
    for(Category cat: ANZEIGEREIHENFOLGE) {
        if (sum.value(cat) > 0) titelSeite += help.arg(toString(cat), minutesToHourString(sum.value(cat)));
    }
    titelSeite += "</ul><ul>";
    titelSeite += help.arg(toString(Gesamt), minutesToHourString(sum.value(Gesamt)));
    titelSeite += help.arg(toString(Kilometer), QString("%1 km").arg(sum.value(Kilometer)));

    titelSeite += "</ul><h2>Mindeststunden</h2><ul>";
    if (getMinimumHours(Gesamt) > 0) titelSeite += help.arg("Insgesamt", minutesToHourString(getMinimumHours(Gesamt)));
    for (Category cat: ANZEIGEREIHENFOLGE) {
        if (getMinimumHours(cat) > 0) titelSeite += help.arg(toString(cat), minutesToHourString(getMinimumHours(cat)));
    }
    titelSeite += "</ul>";

    titelSeite += Export::zeitStempel(true);

    return Export::druckeHtmlAufDrucker(titelSeite+a, printer);
}

bool ManagerPersonal::printZeitenListe(QList<Person *> personen, QSet<Category> spalten, Status filter, QPrinter *printer)
{
    QString a = "<h3>Einsatzzeiten: %1</h3>"
                "<table cellspacing='0' width='100%'><thead><tr> <th>Name</th>";
    a = a.arg(toString(filter));
    for (Category cat: ANZEIGEREIHENFOLGEGESAMT) {
        if (! spalten.contains(cat)) continue;
        switch (cat) {
        case Zub:
        case Begleiter:
            a += "<th>Zub/Begl.o.b.A.</th>"; break;
        default:
            a += "<th>"+toString(cat)+"</th>";
        }
    }
    a += "</thead><tbody>";

    QMap<Category, int> sum;
    for(Person *p: personen) {
        a += p->getZeitenFuerListeAlsHTML(spalten);
        for (Category cat: qAsConst(spalten)) {
            sum.insert(cat, sum.value(cat,0)+p->getZeiten(cat));
        }
    }
    a += "</tbody><tfoot><tr>";
    a += "<td>Summe:</td>";
    for (Category cat: ANZEIGEREIHENFOLGEGESAMT) {
        if (! spalten.contains(cat)) continue;
        switch (cat) {
        case Category::Anzahl:
        case Category::Kilometer:
            a += "<td align='right'>"+QString::number(sum.value(cat))+"</td>";
            break;
        default:
            a += "<td align='right'>"+minutesToHourStringShort(sum.value(cat))+"</td>";
        }
    }
    a += "</tr></tfoot></table>";
    a += Export::zeitStempel(false);

    return Export::druckeHtmlAufDrucker(a, printer);
}

bool ManagerPersonal::printMitgliederEinzel(QList<Person *> liste, Status filter, QPrinter *printer) const
{
    QString a = "";
    // Seite fuer jede Person einfuegen
    QMap<Category, int> sum;
    for(Person *p: liste) {
        a += p->getPersonaldatenFuerEinzelAlsHTML();
        a += Export::zeitStempel(p != liste.last());
        for (Category cat: ANZEIGEREIHENFOLGEGESAMT) {
            sum.insert(cat, sum.value(cat,0)+p->getZeiten(cat));
        }
    }

    QString help = "<li>%1: %2</li>";
    QString titelSeite = "<h1>"+QObject::tr("Mitgliederübersicht: %1").arg(toString(filter))+"</h1>";
    titelSeite += "<h2>"+QObject::tr("Statistik")+"</h2><ul>";
    switch (filter) {
    case Registriert:
        titelSeite += help.arg(toString(Registriert)).arg(getAnzahlMitglieder(Registriert));
        [[fallthrough]];
    case AlleMitglieder:
        titelSeite += help.arg(toString(AlleMitglieder)).arg(getAnzahlMitglieder(AlleMitglieder));
        [[fallthrough]];
    case Ausgetreten:
        titelSeite += help.arg(toString(Ausgetreten)).arg(getAnzahlMitglieder(Ausgetreten));
        if (filter == Ausgetreten) break;
        titelSeite += "</ul><br/><ul>";
        [[fallthrough]];
    case Aktiv:
        titelSeite += help.arg(toString(Aktiv)).arg(getAnzahlMitglieder(Aktiv));
        [[fallthrough]];
    case AktivMit:
        titelSeite += help.arg(toString(AktivMit)).arg(getAnzahlMitglieder(AktivMit));
        if (filter == AktivMit) break;
        [[fallthrough]];
    case AktivOhne:
        titelSeite += help.arg(toString(AktivOhne)).arg(getAnzahlMitglieder(AktivOhne));
        if (filter == AktivOhne) break;
        if (filter == Aktiv) break;
        [[fallthrough]];
    case Passiv:
        titelSeite += help.arg(toString(Passiv)).arg(getAnzahlMitglieder(Passiv));
        [[fallthrough]];
    case PassivMit:
        titelSeite += help.arg(toString(PassivMit)).arg(getAnzahlMitglieder(PassivMit));
        if (filter == PassivMit) break;
        [[fallthrough]];
    case PassivOhne:
        break;
    }
    titelSeite += "</ul><h2>" + QObject::tr("Mitgliedsbeiträge")+"</h2><ul>";
    help = "<li>%1: %2€</li>";
    for(auto it = beitraege.cbegin(); it != beitraege.cend(); ++it) {
        if (it.value() != 0)
            titelSeite += help.arg(Person::toString(it.key())).arg(it.value()/100.f, 0, 'f', 2);
    }
    titelSeite += "</ul>";
    titelSeite += Export::zeitStempel(true);

    return Export::druckeHtmlAufDrucker(titelSeite+a, printer);
}

bool ManagerPersonal::printMitgliederListe(QList<Person*> liste, Status filter, QSet<QString> data, QPrinter *printer)
{
    QString a = Person::getKopfTabelleListeHtml(data)
            .arg(toString(filter), QDateTime::currentDateTime().toString("d.M.yyyy"));
    for(Person *akt: liste) {
        a += akt->getPersonaldatenFuerListeAlsHTML(data);
    }
    a += Person::FUSS_TABELLE_LISTE_HTML;
    a += QObject::tr("<p><small>%1 Personen ausgegeben.</small><br/>").arg(liste.length());
    a += QObject::tr("<small>Erstellt am: %1</small></p>").arg(QDateTime::currentDateTime().toString("d.M.yyyy HH:mm"));

    return Export::druckeHtmlAufDrucker(a, printer);
}

bool ManagerPersonal::saveMitgliederListeAlsCSV(QList<Person *> liste, QString pfad)
{
    QString t = Person::KOPF_TABELLE_LISTE_CSV;
    for(Person *akt: liste) {
        t += akt->getPersonaldatenFuerListeAlsCSV();
    }
    return FileIO::saveToFile(pfad, t);
}

int ManagerPersonal::getAnzahlMitglieder(Status filter) const
{
    return getPersonen(filter).length();
}

QList<Person *> ManagerPersonal::getPersonen(Status filter) const
{
    QList<Person *> current = QList<Person*>();
    for(Person *p: getPersonenSortiertNachNummer()) {
        if (p->pruefeFilter(filter))
            current.append(p);
    }
    return current;
}

bool ManagerPersonal::saveBeitraegeRegulaerAlsCSV(QString pfad) const
{
    // Beitraege als CSV erstellen
    QString csv = "Name;IBAN;Bank;Kontoinhaber;Betrag;Mitgliedsnummer\n";
    for(Person *pers: personen) {
        if (pers->getBeitrag() != 0) {
            csv += QString("%1;%2;%3;%4;%5;%6\n")
                    .arg(pers->getName(), pers->getIban(), pers->getBank(), pers->getKontoinhaberFinal())
                    .arg(pers->getBeitrag()/100.f, 0, 'f', 2)
                    .arg(pers->getNummer());
        }
    }
    return FileIO::saveToFile(pfad, csv);
}

bool ManagerPersonal::saveBeitraegeNachzahlungAlsCSV(QString pfad) const
{
    // Beitraege als CSV erstellen
    QString csv = "Name;IBAN;Bank;Kontoinhaber;Betrag;Mitgliedsnummer\n";
    for(Person *pers: personen) {
        if (pers->getBeitragNachzahlung() != 0) {
            csv += QString("%1;%2;%3;%4;%5;%6\n")
                    .arg(pers->getName(), pers->getIban(), pers->getBank(), pers->getKontoinhaberFinal())
                    .arg(pers->getBeitragNachzahlung()/100.f, 0, 'f', 2)
                    .arg(pers->getNummer());
        }
    }
    return FileIO::saveToFile(pfad, csv);
}
