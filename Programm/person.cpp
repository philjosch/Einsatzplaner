#include "personalwindow.h"
#include "managerpersonal.h"
#include "person.h"

#include <QMap>
#include <QListIterator>
#include <QDebug>
#include <QVariant>

Person::Person(QString vorname, QString nachname, ManagerPersonal *manager)
{
    personConstructor(vorname, nachname, manager);
}

Person::Person(QString name, ManagerPersonal *manager) : QObject()
{
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
    p->additionalKilometer = o.value("additionalKilometer").toDouble(0);
    return p;
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
    sumKilometer = 0;

    QDate today = QDate::currentDate();
    for(AActivity *a: activities->keys()) {
        if (a->getDatum() <= today) {
            Category cat = activities->value(a);
            AActivity::Infos *info = a->getIndividual(this);

            // Einsatzstunden
            QTime start = info->beginn;
            QTime ende = info->ende;
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
    sumKilometer += additionalKilometer;

    valuesInvalid = false;
}

bool Person::addActivity(AActivity *a, Category category)
{
    activities->insert(a, category);
    valuesInvalid = true;
    return true;
}

bool Person::removeActivity(AActivity *a)
{
    if (activities->remove(a) != 0) {
        valuesInvalid = true;
        return true;
    }
    return false;
}

QListIterator<AActivity *> *Person::getActivities()
{
    QListIterator<AActivity*> *i = new QListIterator<AActivity*>(activities->keys());
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
QString Person::getHtmlForTableView(QList<bool> *liste)
{
    //  0: summe gesamt
    //  1: anzahl
    //  2: tf/tb
    //  3: zf
    //  4: zub/begl.o.b.a.
    //  5: service
    //  6: zug vorbereiten
    //  7: werkstatt
    //  8: büro
    //  9: sonstiges
    // 10: kilometer
    // 11: Ausbildung
    while (liste->length() < 11) {
        liste->append(0);
    }
    QString html;
    if (manager->pruefeStunden(this)) {
        html = "<tr>";
    } else {
        html = "<tr style='background-color:"+PersonalWindow::nichtGenugStunden+";'>";
    }
    html += "<td>"+getName()+"</td>";
    if (liste->at(0))
        html += "<td>"+QString::number(getTimeSum())+"</td>";
    if (liste->at(1))
        html += "<td>"+QString::number(getAnzahl())+"</td>";
    if (liste->at(2))
        html += "<td>"+QString::number(getTimeTf())+"</td>";
    if (liste->at(3))
        html += "<td>"+QString::number(getTimeZf())+"</td>";
    if (liste->at(4))
        html += "<td>"+QString::number(getTimeZub())+"</td>";
    if (liste->at(5))
        html += "<td>"+QString::number(getTimeService())+"</td>";
    if (liste->at(6))
        html += "<td>"+QString::number(getTimeVorbereiten())+"</td>";
    if (liste->at(7))
        html += "<td>"+QString::number(getTimeWerkstatt())+"</td>";
    if (liste->at(8))
        html += "<td>"+QString::number(getTimeBuero())+"</td>";
    if (liste->at(11))
        html += "<td>"+QString::number(getTimeAusbildung())+"</td>";
    if (liste->at(9))
        html += "<td>"+QString::number(getTimeSonstiges())+"</td>";
    if (liste->at(10))
        html += "<td>"+QString::number(getSumKilometer())+"</td>";
    html += "</tr>";
    return html;
}

QString Person::getHtmlForDetailPage(ManagerPersonal *m)
{
    QString html = "<h3>"+vorname+" "+nachname+"</h3>";
    if (ausbildungRangierer || ausbildungTf || ausbildungZf)
        html = html + "<p>Ausbildung zum: " + (ausbildungTf? "Triebfahrzeugführer, ":" ") + (ausbildungZf? "Zugführer, ":" ")
                + (ausbildungRangierer? "Rangierer":" ") + "<br/>";
    html += "Entfernung zum Bahnhof: "+QString::number(strecke) + "km</p>";
    html += "<h4>Geleistete Stunden</h4>";
    html += "<ul>";
    QString help = "<li %1>%3: %4h%2</li>";
    QString helpcurrent;
    if (timeTf > 0 || ausbildungTf) {
        if (m->checkHours(this, Tf)) helpcurrent = help.arg("", "");
        else helpcurrent = help.arg("style=\"color: red;\"", " (mindestens "+QString::number(m->getMinimumHours(Tf))+"h)");
        html += helpcurrent.arg("Tf").arg(timeTf, 0, 'f', 1);
    }
    if (timeZf > 0 || ausbildungZf) {
        if (m->checkHours(this, Zf)) helpcurrent = help.arg("", "");
        else helpcurrent = help.arg("style=\"color: red;\"", " (mindestens "+QString::number(m->getMinimumHours(Zf))+"h)");
        html += helpcurrent.arg("Zf").arg(timeZf, 0, 'f', 1);
    }
    if (timeZub > 0) {
        if (m->checkHours(this, Zub)) helpcurrent = help.arg("", "");
        else helpcurrent = help.arg("style=\"color: red;\"", " (mindestens "+QString::number(m->getMinimumHours(Zub))+"h)");
        html += helpcurrent.arg("Zub/Begl.o.b.A.").arg(timeZub, 0, 'f', 1);
    }
    if (timeService > 0) {
        if (m->checkHours(this, Service)) helpcurrent = help.arg("", "");
        else helpcurrent = help.arg("style=\"color: red;\"", " (mindestens "+QString::number(m->getMinimumHours(Service))+"h)");
        html += helpcurrent.arg("Service").arg(timeService, 0, 'f', 1);
    }
    if (timeVorbereiten > 0) {
        if (m->checkHours(this, ZugVorbereiten)) helpcurrent = help.arg("", "");
        else helpcurrent = help.arg("style=\"color: red;\"", " (mindestens "+QString::number(m->getMinimumHours(ZugVorbereiten))+"h)");
        html += helpcurrent.arg("Zug Vorbereiten").arg(timeVorbereiten, 0, 'f', 1);
    }
    if (timeWerkstatt > 0) {
        if (m->checkHours(this, Werkstatt)) helpcurrent = help.arg("", "");
        else helpcurrent = help.arg("style=\"color: red;\"", " (mindestens "+QString::number(m->getMinimumHours(Werkstatt))+"h)");
        html += helpcurrent.arg("Werkstatt").arg(timeWerkstatt, 0, 'f', 1);
    }
    if (timeBuero > 0) {
        if (m->checkHours(this, Buero)) helpcurrent = help.arg("", "");
        else helpcurrent = help.arg("style=\"color: red;\"", " (mindestens "+QString::number(m->getMinimumHours(Buero))+"h)");
        html += helpcurrent.arg("Büro").arg(timeBuero, 0, 'f', 1);
    }
    if (timeAusbildung > 0 || ausbildungTf || ausbildungZf || ausbildungRangierer) {
        if (m->checkHours(this, Ausbildung)) helpcurrent = help.arg("", "");
        else helpcurrent = help.arg("style=\"color: red;\"", " (mindestens "+QString::number(m->getMinimumHours(Ausbildung))+"h)");
        html += helpcurrent.arg("Ausbildung").arg(timeAusbildung, 0, 'f', 1);
    }
    if (timeSonstiges > 0) {
        if (m->checkHours(this, Sonstiges)) helpcurrent = help.arg("", "");
        else helpcurrent = help.arg("style=\"color: red;\"", " (mindestens "+QString::number(m->getMinimumHours(Sonstiges))+"h)");
        html += helpcurrent.arg("Sonstiges").arg(timeSonstiges, 0, 'f', 1);
    }

    html += "</ul><ul>";
    if (m->checkHours(this)) helpcurrent = help.arg("", "");
    else helpcurrent = help.arg("style=\"color: red;\"", " (mindestens "+QString::number(m->getMinimumHours())+"h)");
    html += helpcurrent.arg("Gesamte Stundenzahl").arg(timeSum, 0, 'f', 1);
    html += "<li>Anzahl Aktivitäten: "+QString::number(getAnzahl())+"</li>";
    html += "<li>Gefahrene Strecke: "+QString::number(sumKilometer)+" km</li></ul>";
    html += "</p>";
    if (manager->pruefeStunden(this)) {
        html += "<p><br/>Die Person hat die für Sie notwendigen Stunden erbracht!</p>";
    } else {
        html += "<p><br/>Die Person hat die für Sie notwendigen Stunden <b style=\"color: red;\">nicht</b> erbracht!</p>";
    }

    // Hier kommt die liste mit den Arbeitseinsätzen
    if (activities->size() > 0) {
        html += "<h4>Übersicht über die einzelnen Aktivitäten</h4>";
        html += "<table cellspacing='0' width='100%'><thead>";
        html += "<tr><th>Datum, Anlass</th><th>Dienstzeiten</th><th>Aufgabe</th><th>Bemerkung</th></tr></thead><tbody>";
        for(AActivity *a: activities->keys()) {
            AActivity::Infos *info = a->getIndividual(this);
            html += "<tr><td>"+a->getDatum().toString("dd. MM. yyyy")+"<br/>"+a->getKurzbeschreibung()+"<br/>"+a->getAnlass()+"</td><td>"
                    + info->beginn.toString("HH:mm")+"-"+info->ende.toString("HH:mm")+"</td><td>"
                    + AActivity::getStringFromCategory(info->kategorie) + "</td><td>"
                    + info->bemerkung + "</td></tr>";
        }
        html += "</tbody></table>";
    } else {
        html += QString("<p>%1 %2 hat an keinen Aktivitäten teilgenommen!</p>").arg(vorname, nachname);
    }
    html+= "<h5>Zusätzliche nicht in der Tabelle erfassten Stunden</h5><ul>";
    help = "<li>%1: %2h</li>";
    if (additionalTimeTf > 0) html+= help.arg("Tf").arg(additionalTimeTf, 0, 'f', 1);
    if (additionalTimeZf > 0) html+= help.arg("Zf").arg(additionalTimeZf, 0, 'f', 1);
    if (additionalTimeZub > 0) html+= help.arg("Zub").arg(additionalTimeZub, 0, 'f', 1);
    if (additionalTimeService > 0) html+= help.arg("Service").arg(additionalTimeService, 0, 'f', 1);
    if (additionalTimeVorbereiten > 0) html+= help.arg("Vorbereiten").arg(additionalTimeVorbereiten, 0, 'f', 1);
    if (additionalTimeWerkstatt > 0) html+= help.arg("Werkstatt").arg(additionalTimeWerkstatt, 0, 'f', 1);
    if (additionalTimeBuero > 0) html+= help.arg("Büro").arg(additionalTimeBuero, 0, 'f', 1);
    if (additionalTimeAusbildung > 0) html+= help.arg("Ausbildung").arg(additionalTimeAusbildung, 0, 'f', 1);
    if (additionalTimeSonstiges > 0) html+= help.arg("Sonstiges").arg(additionalTimeSonstiges, 0, 'f', 1);
    if (additionalKilometer > 0) html+= QString("<li>Kilometer: %1km</li>").arg(additionalKilometer, 0, 'f', 1);
    html+= "</ul>";

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

double Person::getAdditionalKilometer() const
{
    return additionalKilometer;
}
void Person::setAdditionalKilometer(double value)
{
    additionalKilometer = value;
    valuesInvalid = true;
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
    timeTf = 0.f;
    timeZf = 0.f;
    timeZub = 0.f;
    timeService = 0.f;
    timeBuero = 0.f;
    timeWerkstatt = 0.f;
    timeVorbereiten = 0.f;
    timeAusbildung = 0.f;
    timeSonstiges = 0.f;
    timeSum = 0.f;
    sumKilometer = 0.f;
    additionalTimeTf = 0.f;
    additionalTimeZf = 0.f;
    additionalTimeZub = 0.f;
    additionalTimeService = 0.f;
    additionalTimeBuero = 0.f;
    additionalTimeWerkstatt = 0.f;
    additionalTimeVorbereiten = 0.f;
    additionalTimeAusbildung = 0.f;
    additionalTimeSonstiges = 0.f;
    additionalKilometer = 0.f;
    valuesInvalid = true;

    activities = new QMap<AActivity *, Category>();
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

double Person::getSumKilometer()
{
    if (valuesInvalid) berechne();
    return sumKilometer;
}

double Person::getAnzahl()
{
    return activities->size();
}
