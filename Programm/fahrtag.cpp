#include "fahrtag.h"
#include <QDebug>
#include <QTime>

#include "managerreservierungen.h"

Fahrtag::Fahrtag(QDate *date, ManagerPersonal *p): AActivity(date, p), ManagerReservierungen()
{

//    manager = new ManagerReservierungen();

    art = Fahrtag::Museumszug;
    wichtig = false;
    zeitTf = new QTime(8, 15);
    zeitAnfang = new QTime(8, 45);
    zeitEnde = new QTime(18, 45);
    benoetigeTf = true;
    benoetigeZf = true;
    benoetigeZub = true;
    benoetigeService = true;

    // Listen für Tf, Zf, Zub und Servie müssen noch initalisiert werden
}

Fahrtag::~Fahrtag()
{
    AActivity::~AActivity();
}

QString Fahrtag::getStringFromArt(Fahrtag::Art art)
{
    switch (art) {
    case Museumszug: return "Museumszug";
    case Sonderzug: return "Sonderzug";
    case Gesellschaftssonderzug: return "Gesellschaftssonderzug";
    case Nikolauszug: return "Nikolauszug";
    case Schnupperkurs: return "Ehrenlokführer Schnupperkurs";
    case Bahnhofsfest: return "BAhnhofsfest";
    case ELFundMuseumszug: return "Museumszug mit Schnupperkurs";
    default: return "Sonstiges";
    }
}

QString Fahrtag::getListString()
{
    return datum->toString("dddd dd.MM.yyyy")+" – Fahrtag";
}

QString Fahrtag::getListStringShort() {
    if (anlass.length() == 0) {
        return "Fahrtag";
    }
    return anlass;
}

void Fahrtag::handleActivity(AActivity *a) {
    emit fahrtagModified(a);
}

void Fahrtag::handleEmit()
{
    handleActivity(this);
}

AActivity::Infos *Fahrtag::getIndividual(Person *person)
{
    Infos *neu = new Infos();
    Infos *alt = personen->value(person);
    neu->aufgabe = alt->aufgabe;
    neu->bemerkung = alt->bemerkung;
    neu->kategorie = alt->kategorie;
    neu->beginn = alt->beginn;
    neu->ende = alt->ende;

    if (alt->beginn == QTime(0,0)) {
        if (alt->kategorie == Tf || alt->kategorie == Tb) {
            neu->beginn = QTime(zeitTf->hour(), zeitTf->minute());
        } else {
            neu->beginn = QTime(zeitAnfang->hour(), zeitAnfang->minute());
        }
    }
    if (alt->ende == QTime(0,0)) {
        neu->ende = QTime(zeitEnde->hour(), zeitEnde->minute());
    }
    return neu;
}

QString Fahrtag::getHtmlForSingleView()
{
    QString html = "";
    // Überschrift
    html += "<h1 class='pb'>" +Fahrtag::getStringFromArt(art) + " am " + datum->toString("dddd dd.MM.yyyy")+(wichtig?" WICHTIG!":"")+"</h1>";
    // Anlass
    if (anlass != "") {
        html += "<p><b>Anlass:</b><br/>"+anlass+"</p>";
    }
    // Wagenreihung
    html += "<p><b>Wagenreihung:</b> "+getWagenreihung()+"</p>";
    // Deisntzeiten
    html += "<p><b>Dienstzeiten</b>:<br/>Beginn Tf, Tb: "+zeitTf->toString("hh:mm")+"<br/>Beginn Sonstige: "+zeitAnfang->toString("hh:mm")+"<br/>";
    html += "Ungefähres Dienstende: "+zeitEnde->toString("hh:mm")+"</p>";
    // Personal
    // *Tf, Tb

    QMap<Person*, AActivity::Infos*> tf;
    QMap<Person*, AActivity::Infos*> zf;
    QMap<Person*, AActivity::Infos*> zub;
    QMap<Person*, AActivity::Infos*> begl;
    QMap<Person*, AActivity::Infos*> service;
    QMap<Person*, AActivity::Infos*> sonstige;

    // Aufsplitten der Personen auf die Einzelnen Listen
    for(Person *p: personen->keys()) {
        switch (personen->value(p)->kategorie) {
        case AActivity::Tf:
        case AActivity::Tb: tf.insert(p, personen->value(p)); break;
        case AActivity::Zf: zf.insert(p, personen->value(p)); break;
        case AActivity::Zub: zub.insert(p, personen->value(p)); break;
        case AActivity::Begleiter: begl.insert(p, personen->value(p)); break;
        case AActivity::Service: service.insert(p, personen->value(p)); break;
        default: sonstige.insert(p, personen->value(p)); break;
        }
    }

    html += "<p><b>Triebfahrzeugführer (Tf), Triebfahrzeugbegleiter(Tb)";
    html += (benoetigeTf ?" werden benötigt": "");
    html += ":</b><br/>"+listToString(&tf, " | ")+"</p>";
    if (art != Fahrtag::Schnupperkurs) {
        // *Zf
        html += "<p><b>Zugführer";
        html += (benoetigeZf ?" wird benötigt":"");
        html += ":</b><br/>"+listToString(&zf, " | ")+"</p>";

        // *Zub, Begl.o.b.A
        html += "<p><b>Zugbegleiter und <i>Begleiter ohne betriebliche Ausbildung</i>";
        html += (benoetigeZub ? " werden benötigt":"");
        html += ":</br><br/>";
        html += listToString(&zub, " | ");
        // Begl. o.b.A
        if (! zub.isEmpty() && ! begl.isEmpty())
            html += " | ";
        html += listToString(&begl, " | ") + "</p>";

        // *Service
        html += "<p><b>Service-Personal";
        html += (benoetigeService ?" wird benötigt":"");
        html += ":</b><br/>"+listToString(&service, " | ") +"</p>";
    }
    // *Sonstiges personal
    html += "<p><b>Sonstiges Personal";
    html += (personalBenoetigt ?" wird benötigt":"");
    html += ":</b><br/>"+listToString(&sonstige, " | ") +"</p>";

    // Reservierungen
    if (bemerkungen!= "") {
        html += "<p>Bemerkungen:<br/>"+bemerkungen+"</p>";
    }

    html += "<p><b>Reservierungen:</b><br/>Bereits "+QString::number(getAnzahlBelegt());
    html += (getAnzahlBelegt() == 1 ? " Platz " :  " Plätze ");
    html += "belegt. Noch "+QString::number(getFrei())+" frei.</p>";

    if (getAnzahl() > 0) {
        html += "<table><thead><tr><th>Kontakt</th><th>Sitzplätze</th><th>Ein- und Ausstieg</th><th>Sonstiges</th></tr></thead><tbody>";
        for(Reservierung *r: *reservierungen) {
            html += r->getTableRow();
        }
        html += "</tbody></table>";
    }
    return html;
}

bool Fahrtag::getBenoetigeService() const
{
    return benoetigeService;
}

void Fahrtag::setBenoetigeService(bool value)
{
    benoetigeService = value;
    handleEmit();
}

bool Fahrtag::getBenoetigeZub() const
{
    return benoetigeZub;
}

void Fahrtag::setBenoetigeZub(bool value)
{
    benoetigeZub = value;
    handleEmit();
}

bool Fahrtag::getBenoetigeZf() const
{
    return benoetigeZf;
}

void Fahrtag::setBenoetigeZf(bool value)
{
    benoetigeZf = value;
    handleEmit();
}

bool Fahrtag::getBenoetigeTf() const
{
    return benoetigeTf;
}

void Fahrtag::setBenoetigeTf(bool value)
{
    benoetigeTf = value;
    handleEmit();
}

bool Fahrtag::getWichtig() const
{
    return wichtig;
}

void Fahrtag::setWichtig(bool value)
{
    wichtig = value;
    handleEmit();
}

QTime *Fahrtag::getZeitTf() const
{
    return zeitTf;
}

void Fahrtag::setZeitTf(QTime *value)
{
    zeitTf = value;
    handleEmit();
}

Fahrtag::Art Fahrtag::getArt() const
{
    return art;
}

void Fahrtag::setArt(const Fahrtag::Art &value)
{
    art = value;
    handleEmit();
}
