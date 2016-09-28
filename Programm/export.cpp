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

bool Export::printPerson(Person *p, QPrinter *pdf, QPrinter *paper)
{
    QTextDocument *d = new QTextDocument();
    return print(pdf, paper, d);
}

bool Export::printPersonen(QList<Person *> *personen, QPrinter *pdf, QPrinter *paper)
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
    a += "<table cellspacing='0' width='100%'><thead><tr> <th>Name</th> <th>Summe Stunden</th> <th>Kilometer</th> <th>Anzahl</th> <th>Tf/Tb</th> <th>Zf</th> <th>Service</th> <th>Werkstatt</th> <th>Büro</th> </tr></thead><tbody>";
    for(Person *person: *personen) {
        a += person->getHtmlForTableView();
    }
    a += "</tbody></table>";
    a += "<p><small>Erstellt am: "+QDate::currentDate().toString("d.M.yyyy")+"</small></p>";
    a += "</body></html>";
    d->setHtml(a);
    qDebug() << a;
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

