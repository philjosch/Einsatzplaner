#include "managerpersonal.h"
#include "person.h"

#include <QJsonArray>

const QMap<Category, int> ManagerPersonal::MINIMUM_HOURS_DEFAULT {{Category::Tf, 100*60}, {Gesamt, 10*60}};


ManagerPersonal::ManagerPersonal()
{
    personen = QList<Person*>();
    minimumHours = MINIMUM_HOURS_DEFAULT;
}

ManagerPersonal::ManagerPersonal(QJsonObject o)
{
    personen = QList<Person*>();
    minimumHours = QMap<Category, int>();

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
            for (QString cat: o2.keys()) {
                Category catt = getCategoryFromLocalizedString(cat);
                minimumHours.insert(catt, int(o2.value(cat).toDouble(0)*60));
            }
        }
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
    QJsonArray keys;
    QJsonArray values;
    for (Category cat: minimumHours.keys()) {
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
    for (Category cat: minimumHours.keys()) {
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

QString ManagerPersonal::getZeitenFuerEinzelListeAlsHTML(QList<Person *> liste, Status filter) const
{
    QString a = "";
    // Seite fuer jede Person einfuegen
    QMap<Category, int> sum;
    for(Person *p: liste) {
        a += p->getZeitenFuerEinzelAlsHTML();
        QString help = "<p><small>Stand: %1</small></p>";
        if (p != liste.last()) {
            help = "<div style='page-break-after:always'>" + help + "</div>";
        }
        a += help.arg(QDateTime::currentDateTime().toString("d.M.yyyy HH:mm"));
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

    titelSeite += "<div style='page-break-after:always'><p><small>Erstellt am: "+QDateTime::currentDateTime().toString("d.M.yyyy HH:mm")+"</small></p></div>";

    return titelSeite+a;
}

QString ManagerPersonal::getZeitenFuerListeAlsHTML(QList<Person *> personen, QSet<Category> spalten, Status filter)
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
        for (Category cat: spalten) {
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
    a += QObject::tr("<p><small>Erstellt am: %1</small></p>").arg(QDateTime::currentDateTime().toString("d.M.yyyy HH:mm"));
    return a;
}

QString ManagerPersonal::getMitgliederFuerEinzelListeAlsHTML(QList<Person *> liste, Status filter) const
{
    QString a = "";
    // Seite fuer jede Person einfuegen
    QMap<Category, int> sum;
    for(Person *p: liste) {
        a += p->getPersonaldatenFuerEinzelAlsHTML();
        QString help = "<p><small>Stand: %1</small></p>";
        if (p != liste.last()) {
            help = "<div style='page-break-after:always'>" + help + "</div>";
        }
        a += help.arg(QDateTime::currentDateTime().toString("d.M.yyyy HH:mm"));
        for (Category cat: ANZEIGEREIHENFOLGEGESAMT) {
            sum.insert(cat, sum.value(cat,0)+p->getZeiten(cat));
        }
    }

    QString help = "<li>%1: %2</li>";
    QString titelSeite = QString("<h1>Mitgliederübersicht: %1</h1><ul>").arg(toString(filter));
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
        titelSeite += "</ul><ul>";
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
    titelSeite += "</ul><div style='page-break-after:always'><p><small>Erstellt am: "+QDateTime::currentDateTime().toString("d.M.yyyy HH:mm")+"</small></p></div>";

    return titelSeite+a;
}

QString ManagerPersonal::getMitgliederFuerListeAlsHtml(QList<Person*> liste, Status filter, QSet<QString> data)
{
    QString a = Person::getKopfTabelleListeHtml(data)
            .arg(toString(filter), QDateTime::currentDateTime().toString("d.M.yyyy"));
    for(Person *akt: liste) {
        a += akt->getPersonaldatenFuerListeAlsHTML(data);
    }
    a += Person::FUSS_TABELLE_LISTE_HTML;
    a += QObject::tr("<p><small>%1 Personen ausgegeben.</small><br/>").arg(liste.length());
    a += QObject::tr("<small>Erstellt am: %1</small></p>").arg(QDateTime::currentDateTime().toString("d.M.yyyy HH:mm"));
    return a;
}

QString ManagerPersonal::getMitgliederFuerListeAlsCSV(QList<Person *> liste)
{
    QString t = Person::KOPF_TABELLE_LISTE_CSV;
    for(Person *akt: liste) {
        t += akt->getPersonaldatenFuerListeAlsCSV();
    }
    return t;
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
