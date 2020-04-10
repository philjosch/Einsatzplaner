#include "export.h"
#include "fileio.h"

#include <QEventLoop>
#include <QNetworkReply>
#include <QPrintDialog>
#include <QNetworkAccessManager>
#include <QTemporaryFile>
#include <QSettings>

bool Export::print(AActivity *a, QPrinter *printer)
{
    QList<AActivity*> *liste = new QList<AActivity*>();
    liste->append(a);
    return printSingle(liste, printer);
}

bool Export::printSingle(QList<AActivity *> *liste, QPrinter *printer)
{
    if (printer == nullptr) return false;
    // Für jedes Objekt in der liste eine Einzelansicht erstellen mit der Methode generateSingle(...)

    // Vorbereiten des Druckers für den Ausdruck
    preparePrinterPortrait(printer);


    QTextDocument *d = new QTextDocument();
    d->setDefaultStyleSheet("body{float: none} body,td,th,p { font-size: 80%;}"
                            "table {border-width: 1px; border-style: solid; border-color: black; border-collapse: collapse;}"
                            "p.last { page-break-after: always; }"
                            " table th, table td { border-width: 1px; border-collapse: collapse; padding: 1px; border-style: solid; border-color: black; }");
    d->setDefaultFont(QFont("Arial", 11, QFont::Normal));
    d->setDocumentMargin(0);

    //header zum Dokument hinzufügen
    QString html = "";

    for(AActivity *aa: *liste) {
        html += aa->getHtmlForSingleView();
        if(liste->last() != aa)
            html += "<p class='last'><small>Erstellt am: "+QDateTime::currentDateTime().toString("d.M.yyyy H:mm")+"</small></p>";
        else
            html += "<p><small>Erstellt am: "+QDateTime::currentDateTime().toString("d.M.yyyy H:mm")+"</small></p>";
    }

    d->setHtml(html);
    d->print(printer);
    return true;
}

bool Export::printList(QList<AActivity *> *liste, QPrinter *printer)
{
    if (printer == nullptr) return false;
    // Vorbereiten des Druckers für den Ausdruck
    preparePrinterLandscape(printer);
    QTextDocument *d = new QTextDocument();
    d->setDefaultStyleSheet("body, td, p { font-size: 10px; font-weight: normal !important;}"
                            "table { border-width: 1px; border-style: solid; border-color: black; } "
                            "table th, table td { border-width: 1px; padding: 1px; border-style: solid; border-color: black;}"
                            "table tr, table td { page-break-inside: avoid; }"
                            "ul { -qt-list-indent: 0; margin-top: 0px !important; margin-bottom: 0px !important }"
                            "li { text-indent: 12px; margin-top: 0px !important; margin-bottom: 0px !important; }");
    d->setDefaultFont(QFont("Arial", 10, QFont::Normal));
    d->setDocumentMargin(0);

    QString a = "<h3>Übersicht über die Aktivitäten</h3>"
                "<table cellspacing='0' width='100%'><thead><tr>"
                "<th>Datum, Anlass</th> <th>Tf, Tb</th> <th><u>Zf</u>, Zub, <i>Begl.o.b.A</i></th>"
                "<th>Service</th> <th>Dienstzeiten</th> <th>Sonstiges</th></tr></thead><tbody>";
    for(AActivity *akt: *liste) {
        a += akt->getHtmlForTableView();
    }
    a += "</tbody></table>"
         "<p><small>Erstellt am: "+QDateTime::currentDateTime().toString("d.M.yyyy H:mm")+"</small></p>";
    d->setHtml(a);
    d->print(printer);
    return true;
}

bool Export::printReservierung(Fahrtag *f, QPrinter *printer)
{
    if (f->getAnzahl() == 0 || printer == nullptr) return false;
    // Vorbereiten des Druckers für den Ausdruck
    preparePrinterLandscape(printer);
    QTextDocument *d = new QTextDocument();
    d->setDefaultStyleSheet("body, tr, td, p { font-size: 11px; }"
                            "table { border-width: 1px; border-style: solid; border-color: black; }"
                            "table th, table td { border-width: 1px; padding: 1px; border-style: solid; border-color: black; }"
                            "table tr, table td { page-break-inside: avoid; }"
                            "ul { -qt-list-indent: 0; }"
                            "li { text-indent: 12px; margin-top: 0px !important; margin-bottom: 0px !important; }");
    d->setDefaultFont(QFont("Arial", 11, QFont::Normal));
    d->setDocumentMargin(0);

    QString a = "<h3>";
    a += Fahrtag::getStringFromArt(f->getArt())+" am "+f->getDatum().toString("dddd dd. MM. yyyy");
    a += " - Die Reservierungen</h3>";
    a += "<table cellspacing='0' width='100%'><thead><tr> <th>Name</th> <th>Anzahl</th> <th>Sitzplätze</th> <th>Sonstiges</th></tr></thead><tbody>";
    // Sortieren der Daten nach Wagenreihung
    QStringList wagen = f->getWagenreihung().split(QRegExp("\\s*,\\s*"));
    QList<int> wagenNummern;
    for(QString s: wagen) wagenNummern.append(s.toInt());

    // Sortieren der Reservierungen
    QHash<int, QList<Reservierung*>*> wagenZuRes;
    QSetIterator<Reservierung*> it = f->getReservierungen();
    while(it.hasNext()) {
        Reservierung *r = it.next();
        for(int i: r->getSitzplatz().keys()) {
            if (!wagenZuRes.contains(i))
                wagenZuRes.insert(i, new QList<Reservierung*>());
            int pos = 0;
            QList<Reservierung*> *list= wagenZuRes.value(i);
            list->insert(0, r);
            while (pos+1 < list->length() && list->at(pos)->getName() > list->at(pos+1)->getName()) {
                list->swapItemsAt(pos, pos+1);
                pos++;
            }
        }
        if (r->getSitzplatz().isEmpty()) {
            if (! wagenZuRes.contains(999))
                wagenZuRes.insert(999, new QList<Reservierung*>());
            int pos = 0;
            QList<Reservierung*> *list= wagenZuRes.value(999);
            list->insert(0, r);
            while (pos+1 < list->length() && list->at(pos)->getName() > list->at(pos+1)->getName()) {
                list->swapItemsAt(pos, pos+1);
                pos++;
            }
        }
    }

    for(int wagenNr: wagenNummern) {
        if (wagenZuRes.contains(wagenNr)) {
            a += "<tr><td columnspan='4'><b>Wagen "+QString::number(wagenNr)+":</b></td></tr>";
            for(Reservierung *r: *wagenZuRes.value(wagenNr)) {
                a += r->getHtmlForDetailTable();
            }
        }
    }
    if (wagenZuRes.contains(999)) {
        a += "<tr><td columnspan='4'><b>Reservierungen ohne Sitzplätze:</b></td></tr>";
        for(Reservierung *r: *wagenZuRes.value(999)) {
            a += r->getHtmlForDetailTable();
        }
    }

    a += "</tbody></table>";
    a += "<p><small>Erstellt am: "+QDateTime::currentDateTime().toString("d.M.yyyy H:mm")+"</small></p>";
    d->setHtml(a);
    d->print(printer);
    return true;
}

bool Export::printPerson(ManagerPersonal *m, QPrinter *printer)
{
    if (printer == nullptr) return false;
    preparePrinterPortrait(printer);
    QTextDocument *d = new QTextDocument();
    // Append a style sheet
    d->setDefaultStyleSheet("body, tr, td, p { font-size: 12px; }"
                            "table { border-width: 1px; border-style: solid; border-color: black; }"
                            "table th, table td { border-width: 1px; padding: 1px; border-style: solid; border-color: black; }"
                            "table tr, table td { page-break-inside: avoid; }"
                            "table tfoot tr, table tfoot td { border-width: 2px; }"
                            "ul { -qt-list-indent: 0; }"
                            "li { text-indent: 12px; margin-top: 0px !important; margin-bottom: 0px !important; }");
    d->setDefaultFont(QFont("Arial", 11, QFont::Normal));
    d->setDocumentMargin(0);

    // Add the title page
    m->berechne();
    QString help = "<li>%1: %2</li>";
    QString a = "<h1>Personalübersicht - Gesamt</h1>"
                "<h2>Geleistete Stunden</h2><ul>";
    if (m->getTime(Tf) > 0) a += help.arg("Tf", minutesToHourString(m->getTime(Tf)));
    if (m->getTime(Zf) > 0) a += help.arg("Zf", minutesToHourString(m->getTime(Zf)));
    if (m->getTime(Zub) > 0) a += help.arg("Zub", minutesToHourString(m->getTime(Zub)));
    if (m->getTime(Service) > 0) a += help.arg("Service", minutesToHourString(m->getTime(Service)));
    if (m->getTime(ZugVorbereiten) > 0) a += help.arg("Zug Vorbereiten", minutesToHourString(m->getTime(ZugVorbereiten)));
    if (m->getTime(Werkstatt) > 0) a += help.arg("Werkstatt", minutesToHourString(m->getTime(Werkstatt)));
    if (m->getTime(Buero) > 0) a += help.arg("Büro", minutesToHourString(m->getTime(Buero)));
    if (m->getTime(Ausbildung) > 0) a += help.arg("Ausbildung", minutesToHourString(m->getTime(Ausbildung)));
    if (m->getTime(Infrastruktur) > 0) a += help.arg("Infrastruktur", minutesToHourString(m->getTime(Infrastruktur)));
    if (m->getTime(Sonstiges) > 0) a += help.arg("Sonstiges", minutesToHourString(m->getTime(Sonstiges)));
    a += "</ul>";

    a += "<ul><li>Stunden gesamt: "+QString::number(m->getTime(Gesamt))+"h</li>";
    a += "<li>Gefahrene Kilometer gesamt: "+QString::number(m->getTime(Kilometer))+" km</li></ul>";

    a += "<h2>Mindeststunden</h2><ul>";
    if (m->getMinimumHours(Gesamt) > 0) a += help.arg("Insgesamt", minutesToHourString(m->getMinimumHours(Gesamt)));
    if (m->getMinimumHours(Tf) > 0) a += help.arg("Tf", minutesToHourString(m->getMinimumHours(Tf)));
    if (m->getMinimumHours(Tb) > 0) a += help.arg("Tb", minutesToHourString(m->getMinimumHours(Tb)));
    if (m->getMinimumHours(Zf) > 0) a += help.arg("Zf", minutesToHourString(m->getMinimumHours(Zf)));
    if (m->getMinimumHours(Zub) > 0) a += help.arg("Zub", minutesToHourString(m->getMinimumHours(Zub)));
    if (m->getMinimumHours(Service) > 0) a += help.arg("Service", minutesToHourString(m->getMinimumHours(Service)));
    if (m->getMinimumHours(ZugVorbereiten) > 0) a += help.arg("Zug Vorbereiten", minutesToHourString(m->getMinimumHours(ZugVorbereiten)));
    if (m->getMinimumHours(Werkstatt) > 0) a += help.arg("Werkstatt", minutesToHourString(m->getMinimumHours(Werkstatt)));
    if (m->getMinimumHours(Buero) > 0) a += help.arg("Büro", minutesToHourString(m->getMinimumHours(Buero)));
    if (m->getMinimumHours(Ausbildung) > 0) a += help.arg("Ausbildung", minutesToHourString(m->getMinimumHours(Ausbildung)));
    if (m->getMinimumHours(Infrastruktur) > 0) a += help.arg("Infrastruktur", minutesToHourString(m->getMinimumHours(Infrastruktur)));
    if (m->getMinimumHours(Sonstiges) > 0) a += help.arg("Sonstiges", minutesToHourString(m->getMinimumHours(Sonstiges)));
    a += "</ul>";

    a += "<div style='page-break-after:always'><p><small>Erstellt am: "+QDateTime::currentDateTime().toString("d.M.yyyy HH:mm")+"</small></p></div>";

    // Add a papge for each person
    QListIterator<Person*> iter = m->getPersonen();
    while(iter.hasNext()) {
        Person *akt = iter.next();
        a += akt->getHtmlForDetailPage(m);
        if (iter.hasNext()) {
            a += "<div style='page-break-after:always'>";
            a += "<p><small>Erstellt am: "+QDateTime::currentDateTime().toString("d.M.yyyy HH:mm")+"</small></p></div>";
        } else {
            a += "<p><small>Erstellt am: "+QDateTime::currentDateTime().toString("d.M.yyyy HH:mm")+"</small></p>";
        }
    }
    d->setHtml(a);
    d->print(printer);
    return true;
}

bool Export::printPerson(ManagerPersonal *m, Person *p, QPrinter *printer)
{
    if (p == nullptr || printer == nullptr) return false;
    preparePrinterPortrait(printer);
    QTextDocument *d = new QTextDocument();
    // Append a style sheet
    d->setDefaultStyleSheet("body, tr, td, p { font-size: 12px; }"
                            "table { border-width: 1px; border-style: solid; border-color: black; }"
                            "table th, table td { border-width: 1px; padding: 1px; border-style: solid; border-color: black; }"
                            "table tr, table td { page-break-inside: avoid; }"
                            "table tfoot tr, table tfoot td { border-width: 2px; }"
                            "ul { -qt-list-indent: 0; }"
                            "li { text-indent: 12px; margin-top: 0px !important; margin-bottom: 0px !important; }");
    d->setDefaultFont(QFont("Arial", 11, QFont::Normal));
    d->setDocumentMargin(0);

    // Add the title page
    m->berechne();
    QString a = p->getHtmlForDetailPage(m);
    a += "<p><small>Erstellt am: "+QDateTime::currentDateTime().toString("d.M.yyyy HH:mm")+"</small></p>";
    d->setHtml(a);
    d->print(printer);
    return true;
}

bool Export::printPersonen(QList<Person *> *personen, QMap<Category, int> gesamt, QList<Category> data, QPrinter *printer)
{
    if (printer == nullptr) return false;
    preparePrinterLandscape(printer);
    QTextDocument *d = new QTextDocument();
    d->setDefaultStyleSheet("body, tr, td, p { font-size: 12px; }"
                            "table { border-width: 1px; border-style: solid; border-color: black; }"
                            "table th, table td { border-width: 1px; padding: 1px; border-style: solid; border-color: black; }"
                            "table tr, table td { page-break-inside: avoid; }"
                            "table tfoot tr, table tfoot td { border-width: 2px; }"
                            "ul { -qt-list-indent: 0; }"
                            "li { text-indent: 12px; margin-top: 0px !important; margin-bottom: 0px !important; }");
    d->setDefaultFont(QFont("Arial", 11, QFont::Normal));
    d->setDocumentMargin(0);


    //  0: gesamtstunden
    //  1: anzahl
    //  2: tf/tb
    //  3: zf
    //  4: zub/begl.o.b.a.
    //  5: service
    //  6: zug vorbereiten
    //  7: werkstatt
    //  8: büro
    //  9: ausbildung
    // 10: sonstiges
    // 11: kilometer
    QString a = "<h3>Personalübersicht</h3>"
                "<table cellspacing='0' width='100%'><thead><tr> <th>Name</th>";
    if (data.contains(Category::Gesamt))
        a += "<th>Stunden</th>";
    if (data.contains(Category::Anzahl))
        a += "<th>Anzahl</th>";
    if (data.contains(Tf))
        a += "<th>Tf/Tb</th>";
    if (data.contains(Category::Zf))
        a += "<th>Zf</th>";
    if (data.contains(Category::Zub))
        a += "<th>Zub/Begl.o.b.A.</th>";
    if (data.contains(Category::Service))
        a += "<th>Service</th>";
    if (data.contains(Category::ZugVorbereiten))
        a += "<th>Zug Vorbereiten</th>";
    if (data.contains(Category::Werkstatt))
        a += "<th>Werkstatt</th>";
    if (data.contains(Category::Buero))
        a += "<th>Büro</th>";
    if (data.contains(Category::Ausbildung))
        a += "<th>Ausbildung</th>";
    if (data.contains(Category::Infrastruktur))
        a += "<th>Infrastruktur</th>";
    if (data.contains(Category::Sonstiges))
        a += "<th>Sonstiges</th>";
    if (data.contains(Category::Kilometer))
        a += "<th>Kilometer</th>";
    a += "</thead><tbody>";
    for(Person *p: *personen) {
        a += p->getHtmlForTableView(data);
    }
    a += "</tbody><tfoot><tr>";
    a += "<td>Summe:</td>";
    foreach (Category cat, data) {
        switch (cat) {
        case Category::Anzahl:
        case Category::Kilometer:
            a += "<td align='right'>"+QString::number(gesamt.value(cat))+"</td>";
            break;
        default:
            a += "<td align='right'>"+minutesToHourString(gesamt.value(cat)).chopped(2)+"</td>";
        }
    }
    a += "</tr></tfoot></table>";
    a += "<p><small>Erstellt am: "+QDateTime::currentDateTime().toString("d.M.yyyy HH:mm")+"</small></p>";
    d->setHtml(a);

    d->print(printer);
    return true;
}

bool Export::printMitglieder(QList<Person *> p, QPrinter *printer)
{
    if (printer == nullptr) return false;
    preparePrinterLandscape(printer);
    QTextDocument *d = new QTextDocument();
    d->setDefaultStyleSheet("body, tr, td, p { font-size: 12px; }"
                            "table { border-width: 1px; border-style: solid; border-color: black; }"
                            "table th, table td { border-width: 1px; padding: 1px; border-style: solid; border-color: black; }"
                            "table tr, table td { page-break-inside: avoid; }"
                            "table tfoot tr, table tfoot td { border-width: 2px; }"
                            "ul { -qt-list-indent: 0; }"
                            "li { text-indent: 12px; margin-top: 0px !important; margin-bottom: 0px !important; }");
    d->setDefaultFont(QFont("Arial", 11, QFont::Normal));
    d->setDocumentMargin(0);

    QString a = "<h3>Mitgliederliste – Stand "+QDateTime::currentDateTime().toString("d.M.yyyy HH:mm")+"</h3>"
                "<table cellspacing='0' width='100%'><thead><tr>"
                "<th>Name<br/>Mitgliedsnummer</th>"
                "<th>Geburtsdatum<br/>Eintritt<br/>Beruf</th>"
                "<th>Anschrift<br/>E-Mail<br/>Telefon</th>"
                "<th>Betriebsdienst</th>"
                "<th>Sonstiges</th>";
    a += "</thead><tbody>";
    foreach(Person *akt, p) {
        a += akt->getHtmlForMitgliederListe();
    }

    a += "</tbody></table>";
    a += "<p><small>Erstellt am: "+QDateTime::currentDateTime().toString("d.M.yyyy HH:mm")+"</small></p>";
    d->setHtml(a);

    d->print(printer);
    return true;
}

bool Export::csvMitglieder(QList<Person *> p, QString path)
{
    QString t = "Nummer;Nachname;Vorname;Geburtsdatum;Eintritt;Status;Austritt;Tf;Zf;Rangierer;Tauglichkeit;Straße;PLZ;Ort;Mail;Zustimmung Mail;Telefon;Zustimmung Telefon;Strecke;Beruf;Bemerkung\n";
    foreach(Person *akt, p) {
        t += akt->getCSV();
    }
    return FileIO::saveToFile(path, t);
}

QPrinter *Export::getPrinterPaper(QWidget *parent)
{
    QPrinter *p = new QPrinter();
    if (QPrintDialog(p, parent).exec() == QDialog::Accepted)
        return p;
    return nullptr;
}

QPrinter *Export::getPrinterPDF(QWidget *parent, QString path)
{
    QString filePath = FileIO::getFilePathSave(parent, path, QObject::tr("PDF-Dateien (*.pdf)"));
    if (filePath == "") return nullptr;
    QPrinter *p = new QPrinter(QPrinter::PrinterResolution);
    p->setOutputFormat(QPrinter::PdfFormat);
    p->setOutputFileName(filePath);
    return p;
}

bool Export::testServerConnection(ManagerFileSettings *settings)
{
    QString server = settings->getServer();
    QString path = settings->getPath();
    QString id = settings->getId();
    // create custom temporary event loop on stack
    QEventLoop eventLoop;

    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    // the HTTP request
    QString request = server+"/"+path+"?id="+id;
    QNetworkRequest req(request);
    QNetworkReply *reply = mgr.get(req);
    eventLoop.exec(); // blocks stack until "finished()" has been called

    QString s = "";
    if (reply->error() == QNetworkReply::NoError) {
        s = QString(reply->readAll());
    }
    delete reply;
    return (s == "OK");
}

bool Export::uploadToServer(ManagerFileSettings *settings, QList<AActivity *> *liste)
{
    if (! settings->getEnabled()) return true;

    /* ERSTELLEN DER DATEI */
    QTemporaryFile tempFile;
    tempFile.open();
    QString localFile = tempFile.fileName();
    QPrinter *p = new QPrinter(QPrinter::PrinterResolution);
    p->setOutputFormat(QPrinter::PdfFormat);
    p->setOutputFileName(localFile);

    printList(liste, p);

    /* HOCHLADEN DER DATEI AUF DEN SERVER */
    QString server = settings->getFullServer();
    QString id = settings->getId();

    QEventLoop eventLoop;

    QNetworkAccessManager am;
    QObject::connect(&am, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    QNetworkRequest request(server);

    QByteArray data;
    data.append("--margin\r\n");
    data.append("Content-Disposition: form-data; name='id'\r\n\r\n"+id+"\r\n");
    data.append("--margin\r\n");
//    data.append("Content-Disposition: form-data; name='action'\r\n\r\n");
//    data.append("--margin\r\n");
    data.append("Content-Disposition: form-data; name='uploaded'; filename='"+id+".pdf'\r\n"); //name of the input is "uploaded" in my form, next one is a file name.
    data.append("Content-Type: application/pdf\r\n\r\n");
    data.append(tempFile.readAll());
    data.append("\r\n");
    data.append("--margin--\r\n"); //closing boundary according to rfc 1867
    request.setRawHeader(QString("Content-Type").toUtf8(), QString("multipart/form-data; boundary=margin").toUtf8());
    request.setRawHeader(QString("Content-Length").toUtf8(), QString::number(data.length()).toUtf8());

    QNetworkReply *reply = am.post(request,data);
    eventLoop.exec();

    QString s = "";
    if (reply->error() == QNetworkReply::NoError) {
        s = QString(reply->readAll());
    }
    delete reply;
    return (s == "OK");
}

int Export::autoUploadToServer(ManagerFileSettings *settings, Manager *mgr)
{
    /* EINSTELLUNGEN LESEN */
    QSettings s;
    if (!s.value("online/useautoupload").toBool()) return -1;
    if (!settings->getAutom()) return -1;

    /* LISTE MIT DEN AKTIVITAETEN ERSTELLEN */
    QList<AActivity *> *liste = new QList<AActivity*>();
    QListIterator<AActivity*> iter = mgr->getActivities();
    while(iter.hasNext()) {
        AActivity *a = iter.next();
        // Beginndatum
        if (settings->getStartdate() == "tdy") {
            if (a->getDatum() < QDate::currentDate()) {
                continue;
            }
        } else if (settings->getStartdate() == "all") {

        } else if (settings->getStartdate() == "bgn") {
            if (a->getDatum().year() < QDate::currentDate().year()) {
                continue;
            }
        }
        // Enddatum
        if (settings->getEnddate() == "p1w") {
            QDate ref = QDate::currentDate().addDays(7); // naechste Woche
            ref = ref.addDays(7-ref.dayOfWeek()); // Ende der Woche
            if (a->getDatum() > ref) {
                continue;
            }
        } else if (settings->getEnddate() == "p1m") {
            QDate ref = QDate::currentDate().addMonths(1); // naechster Monat
            ref = QDate(ref.year(), ref.month(), ref.daysInMonth()); // Ende des Monats
            if (a->getDatum() > ref) {
                continue;
            }
        } else if (settings->getEnddate() == "eoy") {
            if (a->getDatum().year() > QDate::currentDate().year()) {
                continue;
            }
        } else if (settings->getEnddate() == "all") {

        }
        // Auch Aktivitaeten?
        if (!settings->getActivities()) {
            if (const Fahrtag *f = dynamic_cast<const Fahrtag*>(a)) {
            } else {
                continue;
            }
        }
        liste->append(a);
    }

    /* UPLOADFUNKTION NUTZEN; UM DATEIEN HOCHZULADEN */
    if (liste->length() == 0)
        return -1;
    if (uploadToServer(settings, liste))
        return 1;
    else
        return 0;
}

void Export::preparePrinterPortrait(QPrinter *p)
{
    if (p == nullptr) return;
    p->setFullPage(true);
    p->setPageMargins(20, 15, 15, 15, QPrinter::Millimeter);
    p->setPageOrientation(QPageLayout::Portrait);
}

void Export::preparePrinterLandscape(QPrinter *p)
{
    if (p == nullptr) return;
    p->setFullPage(true);
    p->setPageMargins(15, 20, 15, 15, QPrinter::Millimeter);
    p->setPageOrientation(QPageLayout::Landscape);
}

