#include "managerpersonal.h"
#include "person.h"

#include <QJsonArray>

QMap<Category, int> ManagerPersonal::MINIMUM_HOURS_DEFAULT {{Category::Tf, 100*60}, {Gesamt, 10*60}};


ManagerPersonal::ManagerPersonal()
{
    personen = QSet<Person*>();
    personenSorted = QMap<QString, Person*>();
    idToPerson = QMap<QString, Person*>();
    minimumHours = MINIMUM_HOURS_DEFAULT;
    time = QMap<Category,int>();
}

ManagerPersonal::~ManagerPersonal()
{

}

QJsonObject ManagerPersonal::toJson()
{
    // Personen
    QJsonArray array;
    for(Person *p: getPersonenSortiertNachNummer()) {
        array.append(p->toJson());
    }
    // Mindesstunden
    QJsonArray keys;
    QJsonArray values;
    foreach (Category cat, minimumHours.keys()) {
        if (minimumHours.value(cat) > 0) {
            keys.append(int(cat));
            values.append(minimumHours.value(cat));
        }
    }

    QJsonObject o;
    o.insert("personen", array);
    o.insert("minimumKeys", keys);
    o.insert("minimumValues", values);
    return o;
}

QJsonObject ManagerPersonal::personalToJson()
{
    // Personen
    QJsonArray array;
    for(Person *p: personen.values()) {
        array.append(p->personalToJson());
    }
    // Mindesstunden
    QJsonArray keys;
    QJsonArray values;
    foreach (Category cat, minimumHours.keys()) {
        if (minimumHours.value(cat) > 0) {
            keys.append(int(cat));
            values.append(minimumHours.value(cat));
        }
    }

    QJsonObject o;
    o.insert("personen", array);
    o.insert("minimumKeys", keys);
    o.insert("minimumValues", values);
    return o;
}

void ManagerPersonal::fromJson(QJsonObject o)
{
    QJsonArray a = o.value("personen").toArray();
    for(int i = 0; i < a.size(); i++) {
        Person *neu = new Person(a.at(i).toObject(), this);
        personen.insert(neu);
        personenSorted.insert(neu->getName(), neu);
        idToPerson.insert(neu->getId(), neu);
        connect(neu, SIGNAL(nameChanged(Person*,QString)), this, SLOT(personChangedName(Person*,QString)));
        connect(neu, &Person::changed, this, [=]() { emit changed();});
    }
    if (o.contains("minimumKeys") && o.contains("minimumValues")) {
        minimumHours.clear();
        QJsonArray keys = o.value("minimumKeys").toArray();
        QJsonArray values = o.value("minimumValues").toArray();
        for (int i = 0; i < keys.size() && i < values.size() ; i++) {
            minimumHours.insert(Category(keys.at(i).toInt()), values.at(i).toInt());
        }
    } else {
        minimumHours.clear();
        minimumHours.insert(Gesamt, int(o.value("minimumTotal").toDouble(0)*60));
        if (o.contains("minimumHours")) {
            QJsonObject o2 = o.value("minimumHours").toObject();
            foreach (QString cat, o2.keys()) {
                Category catt = getCategoryFromLocalizedString(cat);
                minimumHours.insert(catt, int(o2.value(cat).toDouble(0)*60));
            }
        }
    }
}

Person *ManagerPersonal::getPersonFromID(QString id)
{
    return idToPerson.value(id);
}

Person *ManagerPersonal::getPerson(QString name)
{
    name = getGoodName(name);
    if (personExists(name)) {
        return personenSorted.value(name);
    }
    return nullptr;
}

bool ManagerPersonal::personExists(QString name)
{
    name = getGoodName(name);
    return personenSorted.contains(name);
}

bool ManagerPersonal::personExists(QString vorname, QString nachname)
{
    while (vorname.endsWith(" ")) vorname.chop(1);
    while (vorname.startsWith(" ")) vorname = vorname.remove(0, 1);
    while (nachname.endsWith(" ")) nachname.chop(1);
    while (nachname.startsWith(" ")) nachname = nachname.remove(0, 1);
    QString nameKomplett;
    if (vorname != "") nameKomplett = vorname + " " + nachname;
    else nameKomplett = nachname;
    return personenSorted.contains(nameKomplett);
}

Person *ManagerPersonal::newPerson()
{
    QString name = tr("Unbekannt Unbenannt");
    if (personExists(name)) return nullptr;
    Person *neu = new Person(name, this);
    personen.insert(neu);
    personenSorted.insert(name, neu);
    idToPerson.insert(neu->getId(), neu);
    connect(neu, SIGNAL(nameChanged(Person*,QString)), this, SLOT(personChangedName(Person*,QString)));
    connect(neu, &Person::changed, this, [=]() { emit changed();});
    emit changed();
    return neu;
}

bool ManagerPersonal::removePerson(Person *p)
{
    if (personen.contains(p)) {
        personen.remove(p);
        personenSorted.remove(p->getName());
        idToPerson.remove(p->getId());
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
int ManagerPersonal::getMinimumHours(Category cat)
{
    return minimumHours.value(cat, 0);
}

int ManagerPersonal::getMinimumHoursDefault(Category cat)
{
    return MINIMUM_HOURS_DEFAULT.value(cat, 0);
}

void ManagerPersonal::personChangedName(Person *p, QString alt)
{
    personenSorted.remove(alt);
    personenSorted.insert(p->getName(), p);
}

QList<Person *> ManagerPersonal::getPersonenSortiertNachNummer()
{
    QList<Person *> l;
    int i = 0;
    foreach (Person *p, personen) {
        l.append(p);
        i = l.size()-1;
        while(i > 0 && l.at(i-1)->getNummer() > l.at(i)->getNummer()) {
            l.swapItemsAt(i, i-1);
            i--;
        }
    }
    return l;
}

void ManagerPersonal::berechne()
{
    time.clear();
    foreach (Person *p, personenSorted.values()) {
        p->berechne();
        foreach(Category cat, ANZEIGEREIHENFOLGEGESAMT) {
            time.insert(cat, time.value(cat)+p->getZeiten(cat));
        }
    }
    time.insert(Anzahl, 0);
}

int ManagerPersonal::getZeiten(Category kat)
{
    return time.value(kat, 0);
}

QString ManagerPersonal::getGoodName(QString name)
{
    if (name.contains(QRegExp("\\s*,\\s*"))) {
        QStringList liste = name.split(QRegExp("\\s*,\\s*"));
        name = liste.at(1) + " " + liste.at(0);
    }
    return name;
}

int ManagerPersonal::getNextNummer()
{
    int max = 0;
    foreach (Person *p, personen.values()) {
        if (p->getNummer() > max) max = p->getNummer();
    }
    return max+1;
}

bool ManagerPersonal::checkNummer(int neu)
{
    QSet<Person*>::iterator i;
    for(i= personen.begin(); i != personen.end(); ++i) {
        if ((*i)->getNummer() == neu) return false;
    }
    return true;
}

QString ManagerPersonal::getZeitenFuerEinzelListeAlsHTML(QList<Person *> liste, Mitglied filter)
{
    berechne();
    QString a = "";
    // Seite fuer jede Person einfuegen
    QMap<Category, int> sum;
    foreach(Person *p, liste) {
        a += p->getZeitenFuerEinzelAlsHTML();
        QString help = "<p><small>Stand: %1</small></p>";
        if (p != liste.last()) {
            help = "<div style='page-break-after:always'>" + help + "</div>";
        }
        a += help.arg(QDateTime::currentDateTime().toString("d.M.yyyy HH:mm"));
        foreach (Category cat, ANZEIGEREIHENFOLGEGESAMT) {
            sum.insert(cat, sum.value(cat,0)+p->getZeiten(cat));
        }
    }

    QString help = "<li>%1: %2</li>";
    QString titelSeite = QString("<h1>Einsatzzeiten: %1</h1><h2>Geleistete Stunden</h2><ul>").arg(getStringVonFilter(filter));
    foreach(Category cat, ANZEIGEREIHENFOLGE) {
        if (sum.value(cat) > 0) titelSeite += help.arg(getLocalizedStringFromCategory(cat), minutesToHourString(sum.value(cat)));
    }
    titelSeite += "</ul><ul>";
    titelSeite += help.arg(getLocalizedStringFromCategory(Gesamt), minutesToHourString(sum.value(Gesamt)));
    titelSeite += help.arg(getLocalizedStringFromCategory(Kilometer), QString("%1 km").arg(sum.value(Kilometer)));

    titelSeite += "</ul><h2>Mindeststunden</h2><ul>";
    if (getMinimumHours(Gesamt) > 0) titelSeite += help.arg("Insgesamt", minutesToHourString(getMinimumHours(Gesamt)));
    foreach (Category cat, ANZEIGEREIHENFOLGE) {
        if (getMinimumHours(cat) > 0) titelSeite += help.arg(getLocalizedStringFromCategory(cat), minutesToHourString(getMinimumHours(cat)));
    }
    titelSeite += "</ul>";

    titelSeite += "<div style='page-break-after:always'><p><small>Erstellt am: "+QDateTime::currentDateTime().toString("d.M.yyyy HH:mm")+"</small></p></div>";

    return titelSeite+a;
}

QString ManagerPersonal::getZeitenFuerListeAlsHTML(QList<Person *> personen, QSet<Category> spalten, Mitglied filter)
{
    QString a = "<h3>Einsatzzeiten: %1</h3>"
                "<table cellspacing='0' width='100%'><thead><tr> <th>Name</th>";
    a = a.arg(getStringVonFilter(filter));
    foreach (Category cat, ANZEIGEREIHENFOLGEGESAMT) {
        if (! spalten.contains(cat)) continue;
        switch (cat) {
        case Tf:
        case Tb:
            a += "<th>Tf/Tb</th>"; break;
        case Zub:
        case Begleiter:
            a += "<th>Zub/Begl.o.b.A.</th>"; break;
        default:
            a += "<th>"+getLocalizedStringFromCategory(cat)+"</th>";
        }
    }
    a += "</thead><tbody>";

    QMap<Category, int> sum;
    for(Person *p: personen) {
        a += p->getZeitenFuerListeAlsHTML(spalten);
        foreach (Category cat, spalten) {
            sum.insert(cat, sum.value(cat,0)+p->getZeiten(cat));
        }
    }
    a += "</tbody><tfoot><tr>";
    a += "<td>Summe:</td>";
    foreach (Category cat, ANZEIGEREIHENFOLGEGESAMT) {
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
    a += QObject::tr("<p><small>Erstellt am: %1</small></p>").arg(QDateTime::currentDateTime().toString("d.M.yyyy HH:mm"));
    return a;
}

QString ManagerPersonal::getMitgliederFuerEinzelListeAlsHTML(QList<Person *> liste, Mitglied filter)
{
    QString a = "";
    // Seite fuer jede Person einfuegen
    QMap<Category, int> sum;
    foreach(Person *p, liste) {
        a += p->getPersonaldatenFuerEinzelAlsHTML();
        QString help = "<p><small>Stand: %1</small></p>";
        if (p != liste.last()) {
            help = "<div style='page-break-after:always'>" + help + "</div>";
        }
        a += help.arg(QDateTime::currentDateTime().toString("d.M.yyyy HH:mm"));
        foreach (Category cat, ANZEIGEREIHENFOLGEGESAMT) {
            sum.insert(cat, sum.value(cat,0)+p->getZeiten(cat));
        }
    }

    QString help = "<li>%1: %2</li>";
    QString titelSeite = QString("<h1>Mitgliederübersicht: %1</h1><ul>").arg(getStringVonFilter(filter));
    switch (filter) {
    case Registriert:
        titelSeite += help.arg(getStringVonFilter(Registriert)).arg(getAnzahlMitglieder(Registriert));
        [[fallthrough]];
    case AlleMitglieder:
        titelSeite += help.arg(getStringVonFilter(AlleMitglieder)).arg(getAnzahlMitglieder(AlleMitglieder));
        [[fallthrough]];
    case Ausgetreten:
        titelSeite += help.arg(getStringVonFilter(Ausgetreten)).arg(getAnzahlMitglieder(Ausgetreten));
        if (filter == Ausgetreten) break;
        titelSeite += "</ul><ul>";
        [[fallthrough]];
    case Aktiv:
        titelSeite += help.arg(getStringVonFilter(Aktiv)).arg(getAnzahlMitglieder(Aktiv));
        [[fallthrough]];
    case AktivMit:
        titelSeite += help.arg(getStringVonFilter(AktivMit)).arg(getAnzahlMitglieder(AktivMit));
        if (filter == AktivMit) break;
        [[fallthrough]];
    case AktivOhne:
        titelSeite += help.arg(getStringVonFilter(AktivOhne)).arg(getAnzahlMitglieder(AktivOhne));
        if (filter == AktivOhne) break;
        if (filter == Aktiv) break;
        [[fallthrough]];
    case Passiv:
        titelSeite += help.arg(getStringVonFilter(Passiv)).arg(getAnzahlMitglieder(Passiv));
        [[fallthrough]];
    case PassivMit:
        titelSeite += help.arg(getStringVonFilter(PassivMit)).arg(getAnzahlMitglieder(PassivMit));
        if (filter == PassivMit) break;
        [[fallthrough]];
    case PassivOhne:
        break;
    }
    titelSeite += "</ul><div style='page-break-after:always'><p><small>Erstellt am: "+QDateTime::currentDateTime().toString("d.M.yyyy HH:mm")+"</small></p></div>";

    return titelSeite+a;
}

QString ManagerPersonal::getMitgliederFuerListeAlsHtml(QList<Person*> liste, Mitglied filter)
{
    QString a = Person::KOPF_TABELLE_LISTE_HTML
            .arg(getStringVonFilter(filter), QDateTime::currentDateTime().toString("d.M.yyyy"));
    foreach(Person *akt, liste) {
        a += akt->getPersonaldatenFuerListeAlsHTML();
    }
    a += Person::FUSS_TABELLE_LISTE_HTML;
    a += QObject::tr("<p><small>Erstellt am: %1</small></p>").arg(QDateTime::currentDateTime().toString("d.M.yyyy HH:mm"));
    return a;
}

QString ManagerPersonal::getMitgliederFuerListeAlsCSV(QList<Person *> liste)
{
    QString t = Person::KOPF_TABELLE_LISTE_CSV;
    foreach(Person *akt, liste) {
        t += akt->getPersonaldatenFuerListeAlsCSV();
    }
    return t;
}

int ManagerPersonal::getAnzahlMitglieder(Mitglied filter)
{
    int i = 0;
    foreach(Person *akt, personen) {
        if (akt->pruefeFilter(filter))
            ++i;
    }
    return i;
}

QList<Person *> ManagerPersonal::getPersonen(Mitglied filter)
{
    QList<Person *> current = QList<Person*>();
    foreach(Person *p, getPersonenSortiertNachNummer()) {
        if (p->pruefeFilter(filter))
            current.append(p);
    }
    return current;
}
