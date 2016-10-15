#include "export.h"
#include <QDebug>
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
    d->setDocumentMargin(0);
    //header zum Dokument hinzufügen
    QString html = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"><html><head><title>Einsatzplan - Einzelansicht</title></head><body>";
    html += "<style type=\"text/css\">";
    html += "body{float: none} body,td,th,p { font-size: 80%;}";
    html += "table {border-width: 1px; border-style: solid; border-color: black; border-collapse: collapse;} p.last { page-break-after: always; }";
    html +=" table th, table td { border-width: 1px; border-collapse: collapse; padding: 1px; border-style: solid; border-color: black; }";
    html +="</style>";

    for(AActivity *aa: *liste) {
        html += aa->getHtmlForSingleView();
        if(liste->last() != aa)
            html += "<p class='last'><small>Erstellt am: "+QDate::currentDate().toString("d.M.yyyy")+"</small></p>";
        else
            html += "<p><small>Erstellt am: "+QDate::currentDate().toString("d.M.yyyy")+"</small></p>";
    }

    // Footer hinzufügen
    html += "</body></html>";
    d->setHtml(html);
    return print(pdf, paper, d);
}

bool Export::printList(QList<AActivity *> *liste, QPrinter *pdf, QPrinter *paper)
{
    // Vorbereiten des Druckers für den Ausdruck
    preparePrinterLandscape(pdf);
    preparePrinterLandscape(paper);
    QTextDocument *d = new QTextDocument();
    d->setDocumentMargin(0);

    QString a = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"><html><head><title>Einsatzplan - Listenansicht</title></head><body>";
    a += "<style type='text/css'>";
    a += "body, tr, td, p { font-size: 12px; }";
    a += "table { border-width: 1px; border-style: solid; border-color: black; }";
    a += "table th, table td { border-width: 1px; padding: 1px; border-style: solid; border-color: black; }";
    a += "table tr, table td { page-break-inside: avoid; }";
    a += "ul { -qt-list-indent: 0; }";
    a += "li { text-indent: 12px; margin-top: 0px !important; margin-bottom: 0px !important; }";
    a += "</style>";
    a += "<h3>Übersicht über die Aktivitäten</h3>";
    a += "<table cellspacing='0' width='100%'><thead><tr> <th>Datum, Anlass</th> <th>Tf, Tb</th> <th><u>Zf</u>, Zub, <i>Begl.o.b.A</i></th> <th>Service</th> <th>Dienstzeiten</th> <th>Sonstiges</th></tr></thead><tbody>";
    for(AActivity *akt: *liste) {
        a += akt->getHtmlForTableView();
    }
    a += "</tbody></table>";
    a += "<p><small>Erstellt am: "+QDate::currentDate().toString("d.M.yyyy")+"</small></p>";
    a += "</body></html>";
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
    d->setDocumentMargin(0);

    QString a = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"><html><head><title>Fahrtag - Reservierungen</title></head><body>";
    a += "<style type='text/css'>";
    a += "body, tr, td, p { font-size: 12px; }";
    a += "table { border-width: 1px; border-style: solid; border-color: black; }";
    a += "table th, table td { border-width: 1px; padding: 1px; border-style: solid; border-color: black; }";
    a += "table tr, table td { page-break-inside: avoid; }";
    a += "ul { -qt-list-indent: 0; }";
    a += "li { text-indent: 12px; margin-top: 0px !important; margin-bottom: 0px !important; }";
    a += "</style>";
    a += "<h3>";
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
    a += "<p><small>Erstellt am: "+QDate::currentDate().toString("d.M.yyyy")+"</small></p>";
    a += "</body></html>";
    d->setHtml(a);
    return print(pdf, paper, d);
}

bool Export::printPerson(Person *p, QPrinter *pdf, QPrinter *paper)
{
    QTextDocument *d = new QTextDocument();
    return print(pdf, paper, d);
}

bool Export::printPersonen(QList<Person *> *personen, QList<bool> *data, QPrinter *pdf, QPrinter *paper)
{
    preparePrinterLandscape(pdf);
    preparePrinterLandscape(paper);
    QTextDocument *d = new QTextDocument();
    d->setDocumentMargin(0);

    QString a = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"><html><head><title>Einsatzplan - Listenansicht</title></head><body>";
    a += "<style type='text/css'>";
    a += "body, tr, td, p { font-size: 12px; }";
    a += "table { border-width: 1px; border-style: solid; border-color: black; }";
    a += "table th, table td { border-width: 1px; padding: 1px; border-style: solid; border-color: black; }";
    a += "table tr, table td { page-break-inside: avoid; }";
    a += "ul { -qt-list-indent: 0; }";
    a += "li { text-indent: 12px; margin-top: 0px !important; margin-bottom: 0px !important; }";
    a += "</style>";
    a += "<h3>Personalübersicht</h3>";

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
    a += "<table cellspacing='0' width='100%'><thead><tr> <th>Name</th>";
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
    a += "</tbody></table>";
    a += "<p><small>Erstellt am: "+QDate::currentDate().toString("d.M.yyyy")+"</small></p>";
    a += "</body></html>";
    d->setHtml(a);
//    qDebug() << a;
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

