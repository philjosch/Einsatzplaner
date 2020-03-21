#include "personalwindow.h"
#include "managerpersonal.h"
#include "person.h"

#include <QJsonArray>
#include <QRandomGenerator>
#include <algorithm>
#include <cmath>

//Person::Person(QString vorname, QString nachname, ManagerPersonal *manager)
//{
//    personConstructor(vorname, nachname, manager);
//}

//Person::Person(QString vorname, QString nachname, QString id, ManagerPersonal *manager)
//{
//    personConstructor(vorname, nachname, manager, id);
//}

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
    activities = QMap<AActivity *, Category>();

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

    o.insert("mail", mail);
    o.insert("mailOK", mailOK);
    o.insert("tel", telefon);
    o.insert("telOK", telefonOK);

    o.insert("strecke", strecke);
    o.insert("beruf", beruf);
    o.insert("bemerkung", bemerkungen);
    return o;
}

int Person::get(Category cat)
{
    if (valuesInvalid) berechne();
    switch (cat) {
    case Category::Tb:
        return zeiten.value(Tf);
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
}

bool Person::getAusbildungZf() const
{
    return ausbildungZf;
}

void Person::setAusbildungZf(bool value)
{
    ausbildungZf = value;
}

bool Person::getAusbildungRangierer() const
{
    return ausbildungRangierer;
}

void Person::setAusbildungRangierer(bool value)
{
    ausbildungRangierer = value;
}

int Person::getStrecke() const
{
    return strecke;
}

void Person::setStrecke(int value)
{
    strecke = value;
    valuesInvalid = true;
}

QString Person::getMail() const
{
    return mail;
}

void Person::setMail(const QString &value)
{
    mail = value;
}

bool Person::getMailOK() const
{
    return mailOK;
}

void Person::setMailOK(bool value)
{
    mailOK = value;
}

QString Person::getTelefon() const
{
    return telefon;
}

void Person::setTelefon(const QString &value)
{
    telefon = value;
}

bool Person::getTelefonOK() const
{
    return telefonOK;
}

void Person::setTelefonOK(bool value)
{
    telefonOK = value;
}

QString Person::getBeruf() const
{
    return beruf;
}

void Person::setBeruf(const QString &value)
{
    beruf = value;
}

QString Person::getBemerkungen() const
{
    return bemerkungen;
}

void Person::setBemerkungen(const QString &value)
{
    bemerkungen = value;
}

int Person::getNummer() const
{
    return nummer;
}

bool Person::setNummer(int value)
{
    if (manager->checkNummer(value)) {
        nummer = value;
        return true;
    }
    return false;
}

bool Person::isAusgetreten()
{
    if (! austritt.isValid()) return false;
    return (austritt <= QDate::currentDate());
}

void Person::berechne()
{
    zeiten.clear();
    zeiten.insert(Anzahl, activities.size());

    QDate today = QDate::currentDate();
    for(AActivity *a: activities.keys()) {
        if (a->getDatum() <= today) {
            Category cat = activities.value(a);
            AActivity::Infos info = a->getIndividual(this);

            // Einsatzstunden
            QTime start = info.beginn;
            QTime ende = info.ende;
            int duration = (start.msecsTo(ende) / 60000); // in Minuten
            switch (cat) {
            case Tb:
                zeiten.insert(Tf, zeiten.value(Tf)+duration);
                break;
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

bool Person::addActivity(AActivity *a, Category category)
{
    activities.insert(a, category);
    valuesInvalid = true;
    return true;
}

bool Person::removeActivity(AActivity *a)
{
    if (activities.remove(a) != 0) {
        valuesInvalid = true;
        return true;
    }
    return false;
}

QListIterator<AActivity *> *Person::getActivities()
{
    QListIterator<AActivity*> *i = new QListIterator<AActivity*>(activities.keys());
    return i;
}

QString Person::getName() const
{
    QString nameKomplett;
    if (vorname != "") nameKomplett = vorname + " " + nachname;
    else nameKomplett = nachname;
    return nameKomplett;
}

QString Person::getVorname() const
{
    return vorname;
}
void Person::setVorname(const QString &value)
{
    QString old = getName();
    vorname = value;
    emit nameChanged(this, old);
}

QString Person::getNachname() const
{
    return nachname;
}
void Person::setNachname(const QString &value)
{
    QString old = getName();
    nachname = value;
    emit nameChanged(this, old);
}

/*
void Person::setName(const QString &value)
{
    QString old = name;
    name = value;
    emit nameChanged(this, old);
}
*/
QString Person::getHtmlForTableView(QList<Category> liste)
{
    QString html = "<tr><td style='background-color:"+(manager->pruefeStunden(this) ? " " : PersonalWindow::nichtGenugStunden)+"'>"+getName()+"</td>";
    foreach (Category cat, liste) {
        switch (cat) {
        case Category::Anzahl:
        case Category::Kilometer:
            html += "<td align='right' style='background-color:"+(manager->checkHours(this, cat) ? " " : PersonalWindow::nichtGenugStunden)+"'>"+QString::number(get(cat))+"</td>";
            break;
        default:
            html += "<td align='right' style='background-color:"+(manager->checkHours(this, cat) ? " " : PersonalWindow::nichtGenugStunden)+"'>"+getStringFromHours(get(cat)).chopped(2)+"</td>";
        }
    }
    html += "</tr>";
    return html;
}

QString Person::getHtmlForDetailPage(ManagerPersonal *m)
{
    QString html = "<h2>"+vorname+" "+nachname+"</h2>";
    if (ausbildungRangierer || ausbildungTf || ausbildungZf)
        html = html + "<p>Ausbildung zum: " + (ausbildungTf? "Triebfahrzeugführer, ":" ") + (ausbildungZf? "Zugführer, ":" ")
                + (ausbildungRangierer? "Rangierer":" ") + "<br/>";
    html += "Entfernung zum Bahnhof: "+QString::number(strecke) + "km</p>";
    html += "<h3>Geleistete Stunden</h3>";
    html += "<ul>";
    QString help = "<li %1>%3: %4%2</li>";
    QString helpcurrent;
    if (get(Tf) > 0 || ausbildungTf) {
        if (m->checkHours(this, Tf)) helpcurrent = help.arg("", "");
        else helpcurrent = help.arg("style=\"color: red;\"", " (mindestens "+getStringFromHours(m->getMinimumHours(Tf))+")");
        html += helpcurrent.arg("Tf", getStringFromHours(get(Tf)));
    }
    if (get(Zf) > 0 || ausbildungZf) {
        if (m->checkHours(this, Zf)) helpcurrent = help.arg("", "");
        else helpcurrent = help.arg("style=\"color: red;\"", " (mindestens "+getStringFromHours(m->getMinimumHours(Zf))+")");
        html += helpcurrent.arg("Zf", getStringFromHours(get(Zf)));
    }
    if (get(Zub) > 0) {
        if (m->checkHours(this, Zub)) helpcurrent = help.arg("", "");
        else helpcurrent = help.arg("style=\"color: red;\"", " (mindestens "+getStringFromHours(m->getMinimumHours(Zub))+")");
        html += helpcurrent.arg("Zub/Begl.o.b.A.", getStringFromHours(get(Zub)));
    }
    if (get(Service) > 0) {
        if (m->checkHours(this, Service)) helpcurrent = help.arg("", "");
        else helpcurrent = help.arg("style=\"color: red;\"", " (mindestens "+getStringFromHours(m->getMinimumHours(Service))+")");
        html += helpcurrent.arg("Service", getStringFromHours(get(Service)));
    }
    if (get(ZugVorbereiten) > 0) {
        if (m->checkHours(this, ZugVorbereiten)) helpcurrent = help.arg("", "");
        else helpcurrent = help.arg("style=\"color: red;\"", " (mindestens "+getStringFromHours(m->getMinimumHours(ZugVorbereiten))+")");
        html += helpcurrent.arg("Zug Vorbereiten", getStringFromHours(get(ZugVorbereiten)));
    }
    if (get(Werkstatt) > 0) {
        if (m->checkHours(this, Werkstatt)) helpcurrent = help.arg("", "");
        else helpcurrent = help.arg("style=\"color: red;\"", " (mindestens "+getStringFromHours(m->getMinimumHours(Werkstatt))+")");
        html += helpcurrent.arg("Werkstatt", getStringFromHours(get(Werkstatt)));
    }
    if (get(Buero) > 0) {
        if (m->checkHours(this, Buero)) helpcurrent = help.arg("", "");
        else helpcurrent = help.arg("style=\"color: red;\"", " (mindestens "+getStringFromHours(m->getMinimumHours(Buero))+")");
        html += helpcurrent.arg("Büro", getStringFromHours(get(Buero)));
    }
    if (get(Ausbildung) > 0 || ausbildungTf || ausbildungZf || ausbildungRangierer) {
        if (m->checkHours(this, Ausbildung)) helpcurrent = help.arg("", "");
        else helpcurrent = help.arg("style=\"color: red;\"", " (mindestens "+getStringFromHours(m->getMinimumHours(Ausbildung))+")");
        html += helpcurrent.arg("Ausbildung", getStringFromHours(get(Ausbildung)));
    }
    if (get(Sonstiges) > 0) {
        if (m->checkHours(this, Sonstiges)) helpcurrent = help.arg("", "");
        else helpcurrent = help.arg("style=\"color: red;\"", " (mindestens "+getStringFromHours(m->getMinimumHours(Sonstiges))+")");
        html += helpcurrent.arg("Sonstiges", getStringFromHours(get(Sonstiges)));
    }

    html += "</ul><ul>";
    if (m->checkHours(this, Gesamt)) helpcurrent = help.arg("", "");
    else helpcurrent = help.arg("style=\"color: red;\"", " (mindestens "+getStringFromHours(m->getMinimumHours(Gesamt))+")");
    html += helpcurrent.arg("Gesamte Stundenzahl", getStringFromHours(get(Gesamt)));
    html += "<li>Anzahl Aktivitäten: "+QString::number(get(Anzahl))+"</li>";
    html += "<li>Gefahrene Strecke: "+QString::number(get(Kilometer))+"km</li></ul>";
    html += "</p>";
    if (manager->pruefeStunden(this)) {
        html += "<p><br/>Die Person hat die für Sie notwendigen Stunden erbracht!</p>";
    } else {
        html += "<p><br/>Die Person hat die für Sie notwendigen Stunden <b style=\"color: red;\">nicht</b> erbracht!</p>";
    }

    // Hier kommt die liste mit den Arbeitseinsätzen
    html += "<h3>Übersicht über die einzelnen Aktivitäten</h3>";
    if (activities.size() > 0) {
        html += "<table cellspacing='0' width='100%'><thead>";
        html += "<tr><th>Datum, Anlass</th><th>Dienstzeiten</th><th>Aufgabe</th><th>Bemerkung</th></tr></thead><tbody>";
        QList<AActivity*> list = activities.keys();
        AActivity::sort(&list);
        for(int i=0; i<list.length(); i++) {
            AActivity *a = list.at(i);
//            for(AActivity *a: activities.keys()) {
            AActivity::Infos info = a->getIndividual(this);
            html += "<tr><td>"+a->getDatum().toString("dd.MM.yyyy")+"<br/>"+a->getKurzbeschreibung();
            if (a->getAnlass() != a->getKurzbeschreibung() && a->getAnlass() != "")
                html += "<br/>"+a->getAnlass();
            html +="</td><td>"
                 + info.beginn.toString("HH:mm")+"-"+info.ende.toString("HH:mm")+"</td><td>"
                 + AActivity::getStringFromCategory(info.kategorie) + "</td><td>"
                 + info.bemerkung + "</td></tr>";
        }
        html += "</tbody></table>";
    } else {
        html += QString("<p>%1 %2 hat an keinen eingetragenen Aktivitäten teilgenommen!</p>").arg(vorname, nachname);
    }
    help = "<li>%1: %2</li>";
    QString h2 = "";
    if (getAdditional(Tf) > 0) h2 += help.arg("Tf", getStringFromHours(getAdditional(Tf)));
    if (getAdditional(Zf) > 0) h2 += help.arg("Zf", getStringFromHours(getAdditional(Zf)));
    if (getAdditional(Zub) > 0) h2 += help.arg("Zub", getStringFromHours(getAdditional(Zub)));
    if (getAdditional(Service) > 0) h2 += help.arg("Service", getStringFromHours(getAdditional(Service)));
    if (getAdditional(ZugVorbereiten) > 0) h2 += help.arg("Vorbereiten", getStringFromHours(getAdditional(ZugVorbereiten)));
    if (getAdditional(Werkstatt) > 0) h2 += help.arg("Werkstatt", getStringFromHours(getAdditional(Werkstatt)));
    if (getAdditional(Buero) > 0) h2 += help.arg("Büro", getStringFromHours(getAdditional(Buero)));
    if (getAdditional(Ausbildung) > 0) h2 += help.arg("Ausbildung", getStringFromHours(getAdditional(Ausbildung)));
    if (getAdditional(Sonstiges) > 0) h2 += help.arg("Sonstiges", getStringFromHours(getAdditional(Sonstiges)));
    help = "<li>%1: %2%3</li>";
    if (getAdditional(Anzahl) > 0) h2 += help.arg("Zusätzliche Aktivitäten").arg(getAdditional(Anzahl)).arg("");
    if (getAdditional(Kilometer) > 0) h2 += help.arg("Gefahrene Strecke").arg(getAdditional(Kilometer)).arg("km");
    if (h2 != "")
        html+= "<h4>Zusätzliche nicht in der Tabelle erfassten Stunden</h4><ul>"+h2+"</ul>";
    return html;
}

int Person::getAdditional(Category cat)
{
    return additional.value(cat, 0);
}

void Person::setAdditional(Category cat, int value)
{
    additional.insert(cat, value);
}

QString Person::getStringFromHours(double duration)
{
    int hours = int(duration);
    int minutes = int(round(fmod(duration, 1)*60));
    return QString("%1:%2 h").arg(hours).arg(minutes, 2, 10, QChar(48));
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
}

QDate Person::getEintritt() const
{
    return eintritt;
}

void Person::setEintritt(const QDate &value)
{
    eintritt = value;
}

bool Person::getAktiv() const
{
    return aktiv;
}

void Person::setAktiv(bool value)
{
    aktiv = value;
}

QDate Person::getAustritt() const
{
    return austritt;
}

void Person::setAustritt(const QDate &value)
{
    austritt = value;
}

QDate Person::getTauglichkeit() const
{
    return tauglichkeit;
}

void Person::setTauglichkeit(const QDate &value)
{
    tauglichkeit = value;
}

int Person::getAnzahl()
{
    return activities.size();
}
