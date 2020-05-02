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

int ManagerPersonal::pruefeStunden(Person *p)
{
    bool ok = true;
    foreach (Category cat, ANZEIGEREIHENFOLGEGESAMT) {
        ok = ok && checkHours(p, cat);
    }
    if (p->getAktiv() && (! ok))
        return 0;
    else if ((!p->getAktiv()) && ok && p->get(Gesamt) >0)
        return -1;
    else
        return 1;
}

int ManagerPersonal::checkHours(Person *p, Category cat)
{
    if (p->getAktiv()) {
        if (p->get(cat) >= getMinimumHours(cat, p)) {
            return 1;
        } else {
            return 0;
        }
    } else {
        if (p->get(cat)>0) {
            return -1;
        } else {
            return 1;
        }
    }
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

int ManagerPersonal::getMinimumHours(Category cat, Person *p)
{
    if (p->isAusgetreten()) return 0;
    switch (cat) {
    case Tf:
        if (! p->isTauglich(Tf) ) return 0;
        return minimumHours.value(cat, 0);
    case Zf:
        if (! p->isTauglich(Zf)) return 0;
        return minimumHours.value(cat, 0);
    case Ausbildung:
        if ((! p->getAusbildungTf() && ! p->getAusbildungZf() && ! p->getAusbildungRangierer()) || p->isTauglich()) return 0;
        return minimumHours.value(cat, 0);
    default:
        return minimumHours.value(cat, 0);
    }
}

QString ManagerPersonal::getMinimumHoursString(Category cat)
{
    int value = getMinimumHours(cat);
    return QString("%1:%2 h").arg(int(value/60)).arg(value % 60, 2, 10,QLatin1Char('0'));
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

QListIterator<Person *> ManagerPersonal::getPersonen() const
{
    QListIterator<Person*> i(personenSorted.values());
    return i;
}

QList<Person *> ManagerPersonal::getPersonenSortiertNachName()
{
    QList<Person *> l;
    int i = 0;
    foreach (Person *p, personen) {
        l.append(p);
        i = l.size()-1;
        while(i > 0 && l.at(i-1)->getName() > l.at(i)->getName()) {
            l.swapItemsAt(i, i-1);
            i--;
        }
    }
    return l;
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
        time.insert(Tf, time.value(Tf)+p->get(Tf));
        time.insert(Zf, time.value(Zf)+p->get(Zf));
        time.insert(Zub, time.value(Zub)+p->get(Zub));
        time.insert(Service, time.value(Service)+p->get(Service));
        time.insert(Buero, time.value(Buero)+p->get(Buero));
        time.insert(Werkstatt, time.value(Werkstatt)+p->get(Werkstatt));
        time.insert(ZugVorbereiten, time.value(ZugVorbereiten)+p->get(ZugVorbereiten));
        time.insert(Ausbildung, time.value(Ausbildung)+p->get(Ausbildung));
        time.insert(Infrastruktur, time.value(Infrastruktur)+p->get(Infrastruktur));
        time.insert(Sonstiges, time.value(Sonstiges)+p->get(Sonstiges));
        time.insert(Gesamt, time.value(Gesamt)+p->get(Gesamt));
        time.insert(Kilometer, time.value(Kilometer)+p->get(Kilometer));
    }
    time.insert(Anzahl, 0);
}

int ManagerPersonal::getTime(Category kat)
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

QString ManagerPersonal::getHtmlFuerGesamtuebersicht(QList<Person *> personen, QSet<Category> spalten)
{
    QString a = "<h3>Einsatzzeiten Personal</h3>"
                "<table cellspacing='0' width='100%'><thead><tr> <th>Name</th>";
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
        a += p->getHtmlForTableView(spalten);
        foreach (Category cat, spalten) {
            sum.insert(cat, sum.value(cat,0)+p->get(cat));
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
    return a;
}

QString ManagerPersonal::getHtmlFuerEinzelansicht()
{
    berechne();
    QString help = "<li>%1: %2</li>";
    QString a = "<h1>Personalübersicht - Gesamt</h1>"
                "<h2>Geleistete Stunden</h2><ul>";
    foreach(Category cat, ANZEIGEREIHENFOLGE) {
        if (getTime(cat) > 0) a += help.arg(getLocalizedStringFromCategory(cat), minutesToHourString(getTime(cat)));
    }
    a += "</ul>";

    a += "<ul><li>Stunden gesamt: "+QString::number(getTime(Gesamt))+"h</li>";
    a += "<li>Gefahrene Kilometer gesamt: "+QString::number(getTime(Kilometer))+" km</li></ul>";

    a += "<h2>Mindeststunden</h2><ul>";
    if (getMinimumHours(Gesamt) > 0) a += help.arg("Insgesamt", minutesToHourString(getMinimumHours(Gesamt)));
    foreach (Category cat, ANZEIGEREIHENFOLGE) {
        if (getMinimumHours(cat) > 0) a += help.arg(getLocalizedStringFromCategory(cat), minutesToHourString(getMinimumHours(cat)));
    }
    a += "</ul>";

    a += "<div style='page-break-after:always'><p><small>Erstellt am: "+QDateTime::currentDateTime().toString("d.M.yyyy HH:mm")+"</small></p></div>";

    // Add a papge for each person
    QList<Person *> personenSortiertNachName = getPersonenSortiertNachName();
    foreach(Person *akt, personenSortiertNachName) {
        a += akt->getHtmlForDetailPage();
        if (akt != personenSortiertNachName.last()) {
            a += "<div style='page-break-after:always'>";
            a += "<p><small>Stand: "+QDateTime::currentDateTime().toString("d.M.yyyy HH:mm")+"</small></p></div>";
        } else {
            a += "<p><small>Stand: "+QDateTime::currentDateTime().toString("d.M.yyyy HH:mm")+"</small></p>";
        }
    }
    return a;
}

QString ManagerPersonal::getCSVnachNummer()
{
    QString t = "Nummer;Nachname;Vorname;Geburtsdatum;Eintritt;Status;Austritt;Tf;Zf;Rangierer;Tauglichkeit;Straße;PLZ;Ort;Mail;Zustimmung Mail;Telefon;Zustimmung Telefon;Strecke;Beruf;Bemerkung\n";
    foreach(Person *akt, getPersonenSortiertNachNummer()) {
        t += akt->getCSV();
    }
    return t;
}

QString ManagerPersonal::getHtmlFuerMitgliederliste()
{
    QString a = "<h3>Mitgliederliste – Stand "+QDateTime::currentDateTime().toString("d.M.yyyy")+"</h3>"
                "<table cellspacing='0' width='100%'><thead><tr>"
                "<th>Name<br/>Mitgliedsnummer<br/>Status</th>"
                "<th>Geburtsdatum<br/>Eintritt<br/>Beruf</th>"
                "<th>Anschrift</th>"
                "<th>E-Mail<br/>Telefon</th>"
                "<th>Betriebsdienst</th>"
                "<th>Sonstiges</th>"
                "</thead><tbody>";
    foreach(Person *akt, getPersonenSortiertNachNummer()) {
        a += akt->getHtmlForMitgliederListe();
    }
    a += "</tbody></table>";
    return a;
}
