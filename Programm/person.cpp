#include "personalwindow.h"
#include "managerpersonal.h"
#include "person.h"

#include <algorithm>
#include <cmath>

Person::Person(QString vorname, QString nachname, ManagerPersonal *manager)
{
    personConstructor(vorname, nachname, manager);
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

QJsonObject Person::toJson()
{
    QJsonObject o;
    o.insert("vorname", vorname);
    o.insert("nachname", nachname);
    o.insert("ausTf", ausbildungTf);
    o.insert("ausZf", ausbildungZf);
    o.insert("ausRang", ausbildungRangierer);
    o.insert("strecke", strecke);
    o.insert("additionalTf", additionalTimeTf);
    o.insert("additionalZf", additionalTimeZf);
    o.insert("additionalZub", additionalTimeZub);
    o.insert("additionalService", additionalTimeService);
    o.insert("additionalBuero", additionalTimeBuero);
    o.insert("additionalWerkstatt", additionalTimeWerkstatt);
    o.insert("additionalAusbildung", additionalTimeAusbildung);
    o.insert("additionalVorbereiten", additionalTimeVorbereiten);
    o.insert("additionalSonstiges", additionalTimeSonstiges);
    o.insert("additionalAnzahl", additionalAnzahl);
    o.insert("additionalKilometer", additionalKilometer);
    return o;
}

QJsonObject Person::personalToJson()
{
    QJsonObject o;
    o.insert("vorname", vorname);
    o.insert("nachname", nachname);
    o.insert("ausTf", ausbildungTf);
    o.insert("ausZf", ausbildungZf);
    o.insert("ausRang", ausbildungRangierer);
    o.insert("strecke", strecke);
    return o;
}

Person *Person::fromJson(QJsonObject o, ManagerPersonal *manager)
{
    Person *p;
    if (o.contains("name")) {
        p = new Person(o.value("name").toString(), manager);
    } else {
        p = new Person(o.value("vorname").toString(), o.value("nachname").toString(), manager);
    }
    p->ausbildungRangierer = o.value("ausRang").toBool();
    p->ausbildungZf = o.value("ausZf").toBool();
    p->ausbildungTf = o.value("ausTf").toBool();
    p->strecke = o.value("strecke").toInt();
    p->additionalTimeTf = o.value("additionalTf").toDouble(0);
    p->additionalTimeZf = o.value("additionalZf").toDouble(0);
    p->additionalTimeZub = o.value("additionalZub").toDouble(0);
    p->additionalTimeBuero = o.value("additionalBuero").toDouble(0);
    p->additionalTimeService = o.value("additionalService").toDouble(0);
    p->additionalTimeWerkstatt = o.value("additionalWerkstatt").toDouble(0);
    p->additionalTimeVorbereiten = o.value("additionalVorbereiten").toDouble(0);
    p->additionalTimeAusbildung = o.value("additionalAusbildung").toDouble(0);
    p->additionalTimeSonstiges = o.value("additionalSonstiges").toDouble(0);
    p->additionalAnzahl = o.value("additionalAnzahl").toInt();
    p->additionalKilometer = o.value("additionalKilometer").toDouble(0);
    return p;
}

double Person::getTime(Category cat)
{
    switch (cat) {
    case Category::Anzahl:
        return getSumAnzahl();
    case Category::Tf:
    case Category::Tb:
        return getTimeTf();
    case Category::Zf:
        return getTimeZf();
    case Category::Zub:
    case Category::Begleiter:
        return getTimeZub();
    case Category::Service:
        return getTimeService();
    case Category::ZugVorbereiten:
        return getTimeVorbereiten();
    case Category::Werkstatt:
        return getTimeWerkstatt();
    case Category::Buero:
        return getTimeBuero();
    case Category::Ausbildung:
        return getTimeAusbildung();
    case Category::Sonstiges:
        return getTimeSonstiges();
    case Category::Kilometer:
        return getSumKilometer();
    case Category::Gesamt:
        return getTimeSum();

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

void Person::berechne()
{
    timeTf = 0;
    timeZf = 0;
    timeZub = 0;
    timeService = 0;
    timeBuero = 0;
    timeWerkstatt = 0;
    timeVorbereiten = 0;
    timeAusbildung = 0;
    timeSonstiges = 0;
    timeSum = 0;
    sumAnzahl = activities.size();
    sumKilometer = 0;

    QDate today = QDate::currentDate();
    for(AActivity *a: activities.keys()) {
        if (a->getDatum() <= today) {
            Category cat = activities.value(a);
            AActivity::Infos info = a->getIndividual(this);

            // Einsatzstunden
            QTime start = info.beginn;
            QTime ende = info.ende;
            int duration = start.msecsTo(ende);
            switch (cat) {
            case Category::Tb:
            case Category::Tf: timeTf += duration;  break;
            case Category::Zf: timeZf += duration;  break;
            case Category::Begleiter:
            case Category::Zub: timeZub += duration; break;
            case Category::Service: timeService += duration;  break;
            case Category::Buero: timeBuero += duration;  break;
            case Category::Werkstatt: timeWerkstatt += duration;  break;
            case Category::ZugVorbereiten: timeVorbereiten += duration; break;
            case Category::Ausbildung: timeAusbildung += duration; break;
            default: timeSonstiges += duration;
            }
            timeSum += duration;
            if (cat != Category::Buero) sumKilometer += 2*strecke;
        }
    }
    timeTf = timeTf/(3600000) + additionalTimeTf;
    timeZf = timeZf/(3600000) + additionalTimeZf;
    timeZub = timeZub/(3600000) + additionalTimeZub;
    timeService = timeService/(3600000) + additionalTimeService;
    timeBuero = timeBuero/(3600000) + additionalTimeBuero;
    timeWerkstatt = timeWerkstatt/(3600000) + additionalTimeWerkstatt;
    timeVorbereiten = timeVorbereiten/3600000 + additionalTimeVorbereiten;
    timeAusbildung = timeAusbildung/3600000 + additionalTimeAusbildung;
    timeSonstiges = timeSonstiges/3600000 + additionalTimeSonstiges;
    timeSum = timeSum/(3600000) + additionalTimeTf + additionalTimeZf + additionalTimeZub
            + additionalTimeService + additionalTimeBuero + additionalTimeWerkstatt
            + additionalTimeVorbereiten + additionalTimeAusbildung + additionalTimeSonstiges;
    sumAnzahl += additionalAnzahl;
    sumKilometer += additionalKilometer;

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
            html += "<td align='right' style='background-color:"+(manager->checkHours(this, cat) ? " " : PersonalWindow::nichtGenugStunden)+"'>"+QString::number(getTime(cat))+"</td>";
            break;
        default:
            html += "<td align='right' style='background-color:"+(manager->checkHours(this, cat) ? " " : PersonalWindow::nichtGenugStunden)+"'>"+getStringFromHours(getTime(cat)).chopped(2)+"</td>";
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
    if (timeTf > 0 || ausbildungTf) {
        if (m->checkHours(this, Tf)) helpcurrent = help.arg("", "");
        else helpcurrent = help.arg("style=\"color: red;\"", " (mindestens "+getStringFromHours(m->getMinimumHours(Tf))+")");
        html += helpcurrent.arg("Tf", getStringFromHours(timeTf));
    }
    if (timeZf > 0 || ausbildungZf) {
        if (m->checkHours(this, Zf)) helpcurrent = help.arg("", "");
        else helpcurrent = help.arg("style=\"color: red;\"", " (mindestens "+getStringFromHours(m->getMinimumHours(Zf))+")");
        html += helpcurrent.arg("Zf", getStringFromHours(timeZf));
    }
    if (timeZub > 0) {
        if (m->checkHours(this, Zub)) helpcurrent = help.arg("", "");
        else helpcurrent = help.arg("style=\"color: red;\"", " (mindestens "+getStringFromHours(m->getMinimumHours(Zub))+")");
        html += helpcurrent.arg("Zub/Begl.o.b.A.", getStringFromHours(timeZub));
    }
    if (timeService > 0) {
        if (m->checkHours(this, Service)) helpcurrent = help.arg("", "");
        else helpcurrent = help.arg("style=\"color: red;\"", " (mindestens "+getStringFromHours(m->getMinimumHours(Service))+")");
        html += helpcurrent.arg("Service", getStringFromHours(timeService));
    }
    if (timeVorbereiten > 0) {
        if (m->checkHours(this, ZugVorbereiten)) helpcurrent = help.arg("", "");
        else helpcurrent = help.arg("style=\"color: red;\"", " (mindestens "+getStringFromHours(m->getMinimumHours(ZugVorbereiten))+")");
        html += helpcurrent.arg("Zug Vorbereiten", getStringFromHours(timeVorbereiten));
    }
    if (timeWerkstatt > 0) {
        if (m->checkHours(this, Werkstatt)) helpcurrent = help.arg("", "");
        else helpcurrent = help.arg("style=\"color: red;\"", " (mindestens "+getStringFromHours(m->getMinimumHours(Werkstatt))+")");
        html += helpcurrent.arg("Werkstatt", getStringFromHours(timeWerkstatt));
    }
    if (timeBuero > 0) {
        if (m->checkHours(this, Buero)) helpcurrent = help.arg("", "");
        else helpcurrent = help.arg("style=\"color: red;\"", " (mindestens "+getStringFromHours(m->getMinimumHours(Buero))+")");
        html += helpcurrent.arg("Büro", getStringFromHours(timeBuero));
    }
    if (timeAusbildung > 0 || ausbildungTf || ausbildungZf || ausbildungRangierer) {
        if (m->checkHours(this, Ausbildung)) helpcurrent = help.arg("", "");
        else helpcurrent = help.arg("style=\"color: red;\"", " (mindestens "+getStringFromHours(m->getMinimumHours(Ausbildung))+")");
        html += helpcurrent.arg("Ausbildung", getStringFromHours(timeAusbildung));
    }
    if (timeSonstiges > 0) {
        if (m->checkHours(this, Sonstiges)) helpcurrent = help.arg("", "");
        else helpcurrent = help.arg("style=\"color: red;\"", " (mindestens "+getStringFromHours(m->getMinimumHours(Sonstiges))+")");
        html += helpcurrent.arg("Sonstiges", getStringFromHours(timeSonstiges));
    }

    html += "</ul><ul>";
    if (m->checkHours(this)) helpcurrent = help.arg("", "");
    else helpcurrent = help.arg("style=\"color: red;\"", " (mindestens "+getStringFromHours(m->getMinimumHours())+")");
    html += helpcurrent.arg("Gesamte Stundenzahl", getStringFromHours(timeSum));
    html += "<li>Anzahl Aktivitäten: "+QString::number(getSumAnzahl())+"</li>";
    html += "<li>Gefahrene Strecke: "+QString::number(sumKilometer)+"km</li></ul>";
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
    if (additionalTimeTf > 0) h2 += help.arg("Tf", getStringFromHours(additionalTimeTf));
    if (additionalTimeZf > 0) h2 += help.arg("Zf", getStringFromHours(additionalTimeZf));
    if (additionalTimeZub > 0) h2 += help.arg("Zub", getStringFromHours(additionalTimeZub));
    if (additionalTimeService > 0) h2 += help.arg("Service", getStringFromHours(additionalTimeService));
    if (additionalTimeVorbereiten > 0) h2 += help.arg("Vorbereiten", getStringFromHours(additionalTimeVorbereiten));
    if (additionalTimeWerkstatt > 0) h2 += help.arg("Werkstatt", getStringFromHours(additionalTimeWerkstatt));
    if (additionalTimeBuero > 0) h2 += help.arg("Büro", getStringFromHours(additionalTimeBuero));
    if (additionalTimeAusbildung > 0) h2 += help.arg("Ausbildung", getStringFromHours(additionalTimeAusbildung));
    if (additionalTimeSonstiges > 0) h2 += help.arg("Sonstiges", getStringFromHours(additionalTimeSonstiges));
    help = "<li>%1: %2%3</li>";
    if (additionalAnzahl > 0) h2 += help.arg("Zusätzliche Aktivitäten").arg(additionalAnzahl).arg("");
    if (additionalKilometer > 0) h2 += help.arg("Gefahrene Strecke").arg(additionalKilometer, 0, 'f', 1).arg("km");
    if (h2 != "")
        html+= "<h4>Zusätzliche nicht in der Tabelle erfassten Stunden</h4><ul>"+h2+"</ul>";
    return html;
}

double Person::getAdditionalTimeTf() const
{
    return additionalTimeTf;
}
void Person::setAdditionalTimeTf(double value)
{
    additionalTimeTf = value;
    valuesInvalid = true;
}

double Person::getAdditionalTimeZf() const
{
    return additionalTimeZf;
}
void Person::setAdditionalTimeZf(double value)
{
    additionalTimeZf = value;
    valuesInvalid = true;
}

double Person::getAdditionalTimeZub() const
{
    return additionalTimeZub;
}
void Person::setAdditionalTimeZub(double value)
{
    additionalTimeZub = value;
    valuesInvalid = true;
}

double Person::getAdditionalTimeService() const
{
    return additionalTimeService;
}
void Person::setAdditionalTimeService(double value)
{
    additionalTimeService = value;
    valuesInvalid = true;
}

double Person::getAdditionalTimeBuero() const
{
    return additionalTimeBuero;
}
void Person::setAdditionalTimeBuero(double value)
{
    additionalTimeBuero = value;
    valuesInvalid = true;
}

double Person::getAdditionalTimeWerkstatt() const
{
    return additionalTimeWerkstatt;
}
void Person::setAdditionalTimeWerkstatt(double value)
{
    additionalTimeWerkstatt = value;
    valuesInvalid = true;
}

double Person::getAdditionalTimeVorbereiten() const
{
    return additionalTimeVorbereiten;
}
void Person::setAdditionalTimeVorbereiten(double value)
{
    additionalTimeVorbereiten = value;
    valuesInvalid = true;
}

double Person::getAdditionalTimeAusbildung() const
{
    return additionalTimeAusbildung;
}
void Person::setAdditionalTimeAusbildung(double value)
{
    additionalTimeAusbildung = value;
    valuesInvalid = true;
}

double Person::getAdditionalTimeSonstiges() const
{
    return additionalTimeSonstiges;
}
void Person::setAdditionalTimeSonstiges(double value)
{
    additionalTimeSonstiges = value;
    valuesInvalid = true;
}

int Person::getAdditionalAnzahl() const
{
    return additionalAnzahl;
}
void Person::setAdditionalAnzahl(int value)
{
    additionalAnzahl = value;
    valuesInvalid = true;
}

double Person::getAdditionalKilometer() const
{
    return additionalKilometer;
}
void Person::setAdditionalKilometer(double value)
{
    additionalKilometer = value;
    valuesInvalid = true;
}

QString Person::getStringFromHours(double duration)
{
    int hours = int(duration);
    int minutes = int(round(fmod(duration, 1)*60));
    return QString("%1:%2 h").arg(hours).arg(minutes, 2, 10, QChar(48));
}

void Person::personConstructor(QString vorname, QString nachname, ManagerPersonal *manager)
{
    this->manager = manager;
    this->vorname = vorname;
    this->nachname = nachname;
    strecke = 0;
    ausbildungTf = false;
    ausbildungZf = false;
    ausbildungRangierer = false;
    timeTf = 0;
    timeZf = 0;
    timeZub = 0;
    timeService = 0;
    timeBuero = 0;
    timeWerkstatt = 0;
    timeVorbereiten = 0;
    timeAusbildung = 0;
    timeSonstiges = 0;
    timeSum = 0;
    sumAnzahl = 0;
    sumKilometer = 0;
    additionalTimeTf = 0;
    additionalTimeZf = 0;
    additionalTimeZub = 0;
    additionalTimeService = 0;
    additionalTimeBuero = 0;
    additionalTimeWerkstatt = 0;
    additionalTimeVorbereiten = 0;
    additionalTimeAusbildung = 0;
    additionalTimeSonstiges = 0;
    additionalAnzahl = 0;
    additionalKilometer = 0;
    valuesInvalid = true;

    activities = QMap<AActivity *, Category>();
}

double Person::getTimeTf()
{
    if (valuesInvalid) berechne();
    return timeTf;
}

double Person::getTimeZf()
{
    if (valuesInvalid) berechne();
    return timeZf;
}

double Person::getTimeZub()
{
    if (valuesInvalid) berechne();
    return timeZub;
}

double Person::getTimeService()
{
    if (valuesInvalid) berechne();
    return timeService;
}

double Person::getTimeBuero()
{
    if (valuesInvalid) berechne();
    return timeBuero;
}

double Person::getTimeWerkstatt()
{
    if (valuesInvalid) berechne();
    return timeWerkstatt;
}

double Person::getTimeVorbereiten()
{
    if (valuesInvalid) berechne();
    return timeVorbereiten;
}

double Person::getTimeAusbildung()
{
    if (valuesInvalid) berechne();
    return timeAusbildung;
}

double Person::getTimeSonstiges()
{
    if (valuesInvalid) berechne();
    return timeSonstiges;
}

double Person::getTimeSum()
{
    if (valuesInvalid) berechne();
    return timeSum;
}

int Person::getAnzahl()
{
    return activities.size();
}

int Person::getSumAnzahl()
{
    if (valuesInvalid) berechne();
    return sumAnzahl;
}

double Person::getSumKilometer()
{
    if (valuesInvalid) berechne();
    return sumKilometer;
}
