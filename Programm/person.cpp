#include "personalwindow.h"
#include "managerpersonal.h"
#include "person.h"

#include <QMap>
#include <QListIterator>
#include <QDebug>
#include <QVariant>

Person::Person(QString name) : QObject(), QVariant()
{
    this->name = name;
    strecke = 0;
    this->ausbildungTf = false;
    this->ausbildungZf = false;
    this->ausbildungRangierer = false;
    timeTf = 0.f;
    timeZf = 0.f;
    timeZub = 0.f;
    timeService = 0.f;
    timeBuero = 0.f;
    timeWerkstatt = 0.f;
    timeSum = 0.f;
    sumKilometer = 0.f;
    valuesInvalid = true;

    activities = new QMap<AActivity *, AActivity::Category>();
}

QJsonObject Person::toJson()
{
    QJsonObject o;
    o.insert("name", name);
    o.insert("ausTf", ausbildungTf);
    o.insert("ausZf", ausbildungZf);
    o.insert("ausRang", ausbildungRangierer);
    o.insert("strecke", strecke);
    return o;
}

Person *Person::fromJson(QJsonObject o)
{
    Person *p = new Person(o.value("name").toString());
    p->ausbildungRangierer = o.value("ausRang").toBool();
    p->ausbildungZf = o.value("ausZf").toBool();
    p->ausbildungTf = o.value("ausTf").toBool();
    p->strecke = o.value("strecke").toInt();
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
    timeSonstiges = 0;
    timeSum = 0;
    sumKilometer = 0;

    QDate today = QDate::currentDate();
    for(AActivity *a: activities->keys()) {
        if (a->getDatum() <= today) {
            AActivity::Category cat = activities->value(a);
            AActivity::Infos *info = a->getIndividual(this);

            // Einsatzstunden
            QTime start = info->beginn;
            QTime ende = info->ende;
            int duration = start.msecsTo(ende);
            switch (cat) {
            case AActivity::Tb:
            case AActivity::Tf: timeTf += duration;  break;
            case AActivity::Zf: timeZf += duration;  break;
            case AActivity::Begleiter:
            case AActivity::Zub: timeZub += duration; break;
            case AActivity::Service: timeService += duration;  break;
            case AActivity::Buero: timeBuero += duration;  break;
            case AActivity::Werkstatt: timeWerkstatt += duration;  break;
            case AActivity::ZugVorbereiten: timeVorbereiten += duration; break;
            default: timeSonstiges += duration;
            }
            timeSum += duration;
            if (cat != AActivity::Buero) sumKilometer += 2*strecke;
        }
    }
    timeTf = timeTf/(3600000);
    timeZf = timeZf/(3600000);
    timeZub = timeZub/(3600000);
    timeService = timeService/(3600000);
    timeBuero = timeBuero/(3600000);
    timeWerkstatt = timeWerkstatt/(3600000);
    timeVorbereiten = timeVorbereiten/3600000;
    timeSonstiges = timeSonstiges/3600000;
    timeSum = timeSum/(3600000);

    valuesInvalid = false;
}

bool Person::addActivity(AActivity *a, AActivity::Category category)
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
    return name;
}

QString Person::getVorname() const
{
    if (! name.contains(" ")) return name;
    QStringList liste = name.split(" ");
    return liste.at(0);
}

QString Person::getNachname() const
{
    if (! name.contains(" ")) return name;
    QStringList liste = name.split(" ");
    return liste.at(1);
}

void Person::setName(const QString &value)
{
    QString old = name;
    name = value;
    emit nameChanged(this, old);
}

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
    while (liste->length() < 10) {
        liste->append(0);
    }
    QString html;
    if (ManagerPersonal::pruefeStunden(this)) {
        html = "<tr>";
    } else {
        html = "<tr style='background-color:"+PersonalWindow::nichtGenugStunden+";'>";
    }
    html += "<td>"+name+"</td>";
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
    if (liste->at(9))
        html += "<td>"+QString::number(getTimeSonstiges())+"</td>";
    if (liste->at(10))
        html += "<td>"+QString::number(getSumKilometer())+"</td>";
    html += "</tr>";
    return html;
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
