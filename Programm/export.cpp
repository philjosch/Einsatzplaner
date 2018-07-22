#include "export.h"
#include "fileio.h"

bool Export::printFahrtag(Fahrtag *f, QPrinter *pdf, QPrinter *paper)
{
    QList<AActivity*> *liste = new QList<AActivity*>();
    liste->append(f);
    return printSingle(liste, pdf, paper);
}

bool Export::printActivity(Activity *a, QPrinter *pdf, QPrinter *paper)
{
    QList<AActivity*> *liste = new QList<AActivity*>();
    liste->append(a);
    return printSingle(liste, pdf, paper);
}

bool Export::printSingle(QList<AActivity *> *liste, QPrinter *pdf, QPrinter *paper)
{
    // Für jedes Objekt in der liste eine Einzelansicht erstellen mit der Methode generateSingle(...)

    // Vorbereiten des Druckers für den Ausdruck
    preparePrinterPortrait(pdf);
    preparePrinterPortrait(paper);


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
    return print(pdf, paper, d);
}

bool Export::printList(QList<AActivity *> *liste, QPrinter *pdf, QPrinter *paper)
{
    // Vorbereiten des Druckers für den Ausdruck
    preparePrinterLandscape(pdf);
    preparePrinterLandscape(paper);
    QTextDocument *d = new QTextDocument();
    d->setDefaultStyleSheet("body, td, p { font-size: 11px; font-weight: normal !important;}"
                            "table { border-width: 1px; border-style: solid; border-color: black; } "
                            "table th, table td { border-width: 1px; padding: 1px; border-style: solid; border-color: black;}"
                            "table tr, table td { page-break-inside: avoid; }"
                            "ul { -qt-list-indent: 0; }"
                            "li { text-indent: 12px; margin-top: 0px !important; margin-bottom: 0px !important; }");
    d->setDefaultFont(QFont("Arial", 11, QFont::Normal));
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
    return print(pdf, paper, d);
}

bool Export::printReservierung(Fahrtag *f, QPrinter *pdf, QPrinter *paper)
{
    if (f->getAnzahl() == 0) return false;
    // Vorbereiten des Druckers für den Ausdruck
    preparePrinterLandscape(pdf);
    preparePrinterLandscape(paper);
    QTextDocument *d = new QTextDocument();
    d->setDefaultStyleSheet("body, tr, td, p { font-size: 12px; }"
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
        for(int i: r->getSitzplatz()->keys()) {
            if (!wagenZuRes.contains(i))
                wagenZuRes.insert(i, new QList<Reservierung*>());
            int pos = 0;
            QList<Reservierung*> *list= wagenZuRes.value(i);
            list->insert(0, r);
            while (pos+1 < list->length() && list->at(pos)->getName() > list->at(pos+1)->getName()) {
                list->swap(pos, pos+1);
                pos++;
            }
        }
        if (r->getSitzplatz()->isEmpty()) {
            if (! wagenZuRes.contains(999))
                wagenZuRes.insert(999, new QList<Reservierung*>());
            int pos = 0;
            QList<Reservierung*> *list= wagenZuRes.value(999);
            list->insert(0, r);
            while (pos+1 < list->length() && list->at(pos)->getName() > list->at(pos+1)->getName()) {
                list->swap(pos, pos+1);
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
    return print(pdf, paper, d);
}

bool Export::printPerson(ManagerPersonal *m, QPrinter *printer)
{
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
    QString help = "<li>%1: %2h</li>";
    QString a = "<h3>Personalübersicht - Gesamt</h3>"
                "<h4>Geleistete Stunden</h4><ul>";
    if (m->getTime(Tf) > 0) a += help.arg("Tf").arg(m->getTime(Tf), 0, 'f', 0);
    if (m->getTime(Zf) > 0) a += help.arg("Zf").arg(m->getTime(Zf), 0, 'f', 0);
    if (m->getTime(Zub) > 0) a += help.arg("Zub").arg(m->getTime(Zub), 0, 'f', 0);
    if (m->getTime(Service) > 0) a += help.arg("Service").arg(m->getTime(Service), 0, 'f', 0);
    if (m->getTime(ZugVorbereiten) > 0) a += help.arg("Vorbereiten").arg(m->getTime(ZugVorbereiten), 0, 'f', 0);
    if (m->getTime(Werkstatt) > 0) a += help.arg("Werkstatt").arg(m->getTime(Werkstatt), 0, 'f', 0);
    if (m->getTime(Buero) > 0) a += help.arg("Büro").arg(m->getTime(Buero), 0, 'f', 0);
    if (m->getTime(Ausbildung) > 0) a += help.arg("Ausbildung").arg(m->getTime(Ausbildung), 0, 'f', 0);
    if (m->getTime(Sonstiges) > 0) a += help.arg("Sonstiges").arg(m->getTime(Sonstiges), 0, 'f', 0);
    a += "</ul>";

    a += "<ul><li>Stunden gesamt: "+QString::number(m->getTimeSum())+"h</li>";
    a += "<li>Gefahrene Kilometer gesamt: "+QString::number(m->getSumKilometer())+" km</li></ul>";

    help = "<li>%1: %2h</li>";
    a += "<h4>Mindeststunden</h4><ul>";
    if (m->getMinimumHours() > 0) a += help.arg("Insgesamt").arg(m->getMinimumHours(), 0, 'f', 1);
    if (m->getMinimumHours(Tf) > 0) a += help.arg("Tf").arg(m->getMinimumHours(Tf), 0, 'f', 1);
    if (m->getMinimumHours(Tb) > 0) a += help.arg("Tb").arg(m->getMinimumHours(Tb), 0, 'f', 1);
    if (m->getMinimumHours(Zf) > 0) a += help.arg("Zf").arg(m->getMinimumHours(Zf), 0, 'f', 1);
    if (m->getMinimumHours(Zub) > 0) a += help.arg("Zub").arg(m->getMinimumHours(Zub), 0, 'f', 1);
    if (m->getMinimumHours(Service) > 0) a += help.arg("Service").arg(m->getMinimumHours(Service), 0, 'f', 1);
    if (m->getMinimumHours(ZugVorbereiten) > 0) a += help.arg("Vorbereiten").arg(m->getMinimumHours(ZugVorbereiten), 0, 'f', 1);
    if (m->getMinimumHours(Werkstatt) > 0) a += help.arg("Werkstatt").arg(m->getMinimumHours(Werkstatt), 0, 'f', 1);
    if (m->getMinimumHours(Buero) > 0) a += help.arg("Büro").arg(m->getMinimumHours(Buero), 0, 'f', 1);
    if (m->getMinimumHours(Ausbildung) > 0) a += help.arg("Ausbildung").arg(m->getMinimumHours(Ausbildung), 0, 'f', 1);
    if (m->getMinimumHours(Sonstiges) > 0) a += help.arg("Sonstiges").arg(m->getMinimumHours(Sonstiges), 0, 'f', 1);
    a += "</ul>";

    a += "<div style='page-break-after:always'><p><small>Erstellt am: "+QDateTime::currentDateTime().toString("d.M.yyyy HH:mm")+"</small></p></div>";

    // Add a papge for each person
    QSetIterator<Person*> iter = m->getPersonen();
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
    return print(nullptr, printer, d);
}

bool Export::printPersonen(QList<Person *> *personen, QList<double> *gesamt, QList<bool> *data, QPrinter *pdf, QPrinter *paper)
{
    preparePrinterLandscape(pdf);
    preparePrinterLandscape(paper);
    QTextDocument *d = new QTextDocument();
    d->setDefaultStyleSheet("body, tr, td, p { font-size: 20px; }"
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
    //  9: sonstiges
    // 10: kilometer
    QString a = "<h3>Personalübersicht</h3>"
                "<table cellspacing='0' width='100%'><thead><tr> <th>Name</th>";
    while (data->length() < 11) {
        data->append(false);
    }
    if (data->at(0))
        a += "<th>Stunden</th>";
    if (data->at(1))
        a += "<th>Anzahl</th>";
    if (data->at(2))
        a += "<th>Tf/Tb</th>";
    if (data->at(3))
        a += "<th>Zf</th>";
    if (data->at(4))
        a += "<th>Zub/Begl.o.b.A.</th>";
    if (data->at(5))
        a += "<th>Service</th>";
    if (data->at(6))
        a += "<th>Zug Vorbereiten</th>";
    if (data->at(7))
        a += "<th>Werkstatt</th>";
    if (data->at(8))
        a += "<th>Büro</th>";
    if (data->at(9))
        a += "<th>Sonstiges</th>";
    if (data->at(10))
        a += "<th>Kilometer</th>";
    a += "</thead><tbody>";
    for(Person *p: *personen) {
        a += p->getHtmlForTableView(data);
    }
    a += "</tbody><tfoot><tr>";
    a += "<td></td>";
    for(int i = 0; i <= 10; i++) {
        if (data->at(i))
            a += "<td>"+QString::number(gesamt->at(i))+"</td>";
    }
    a += "</tr></tfoot></table>";
    a += "<p><small>Erstellt am: "+QDateTime::currentDateTime().toString("d.M.yyyy HH:mm")+"</small></p>";
    d->setHtml(a);

    return print(pdf, paper, d);
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
    QString filePath = FileIO::getFilePathSave(parent, path, parent->tr("PDF-Dateien (*.pdf)"));
    if (filePath == "") return nullptr;
    QPrinter *p = new QPrinter(QPrinter::PrinterResolution);
    p->setOutputFormat(QPrinter::PdfFormat);
    p->setOutputFileName(filePath);
    return p;
}

bool Export::print(QPrinter *pdf, QPrinter *paper, QTextDocument *d)
{
    if (pdf != nullptr) {
        d->print(pdf);
    }
    if(paper != nullptr) {
        d->print(paper);
    }
    return true;
}

void Export::preparePrinterPortrait(QPrinter *p)
{
    if (p == nullptr) return;
    p->setFullPage(true);
    p->setPageMargins(25, 15, 15, 20, QPrinter::Millimeter);
    p->setPageOrientation(QPageLayout::Portrait);
}

void Export::preparePrinterLandscape(QPrinter *p)
{
    if (p == nullptr) return;
    p->setFullPage(true);
    p->setPageMargins(15, 25, 15, 20, QPrinter::Millimeter);
    p->setPageOrientation(QPageLayout::Landscape);
}

