#include "export.h"
#include <QDebug>

/*Export::Export()
{

}
*/


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
    preparePrinterSingle(pdf);
    preparePrinterSingle(paper);


    QTextDocument *d = new QTextDocument();
    d->setDocumentMargin(0);
    //header zum Dokument hinzufügen
    QString html = "<html><head><title>Einsatzplan - Einzelansicht</title></head><body>";
    html += "<style type=\"text/css\">";
    html += "body{float: none} body,td,th,p { font-size: 80%;}";
    html += "table {border-width: 1px; border-style: solid; border-color: black; border-collapse: collapse;} p.last { page-break-after: always; }";
    html +=" table th, table td { border-width: 1px; border-collapse: collapse; padding: 1px; border-style: solid; border-color: black; }";
    html +="</style>";

    for(AActivity *aa: *liste) {
        html += aa->getHtmlForSingleView();
        html += "<p class='last'><small>Erstellt am: "+QDate::currentDate().toString("d.M.yyyy")+"</small></p>";
    }

    // Footer hinzufügen
    html += "</body></html>";
    d->setHtml(html);
    return print(pdf, paper, d);
}

bool Export::printList(QList<AActivity *> *liste, QPrinter *pdf, QPrinter *paper)
{
    // Vorbereiten des Druckers für den Ausdruck
    preparePrinterList(pdf);
    preparePrinterList(paper);
    QTextDocument *d = new QTextDocument();
    d->setDocumentMargin(0);

    //    d->setPageSize(QSizeF(printer->pageRect().size()));

    QString a = "<html><head><title>Einsatzplan</title></head><body>";
    a += "<style type='text/css'>";
    a += "body, tr, td, p { font-size: 80%;}";
    a += "table {border-width: 1px; border-style: solid; border-color: black; border-collapse: collapse; }";
    a += "table th, table td {border-width: 1px; border-collapse: collapse; padding: 0px;border-style: solid;border-color: black; }";
    a += "table tr { page-break-inside: avoid;}";
    a +="</style>";
    a += "<h3 page-break-before='always'>Übersicht über die Fahrtage</h3>";
    a += "<table><thead><tr> <th>Datum, Anlass</th> <th>Tf, Tb</th> <th><u>Zf</u>, Zub, <i>Begl.o.b.A</i></th> <th>Service</th> <th>Dienstbeginn</th> <th>Sonstiges</th></tr></thead><tbody>";
//    for(AActivity *akt: *liste) {
//        if (Fahrtag *f = dynamic_cast<Fahrtag*>(akt)) {
//            // fahrtag
//            a += "<tr bgcolor='"+MainWindow::getFarbeZug(f->getArt())+"'>";
//            a += (f->getWichtig() ? "<td bgcolor='red'>": "<td>" );
//            a += "<b>"+f->getDatum()->toString("dddd d.M.yyyy")+"</b><br/>("+f->getStringFromArt(f->getArt())+")<br/>"+f->getAnlass()+"</td>";
////            a += (f->getBenoetigeTf()?"<td><b>Lokführer werden benötigt!</b><br/>":"<td>")+listToString(f->getListTf(), "|<br/>")+"</td>";
//            a += "<td></td><td></td>";
///*            if (f->getArt() != 3) {
//                a += (f->getBenoetigeTf()?"<td><u><b>Zugführer wird benötigt!</b><br/>":"<td><u>")+listToString(f->getZf(), "|<br/>")+"</u><br/>";
//                a += (f->getBenoetigeTf()?"<b>Zugbegleiter werden benötigt!</b><br/>":" ")+listToString(f->getZub(), "|<br/>")+"</td>";
//                a += (f->getBenoetigeTf()?"<td><b>Service-Personal wird benötigt!</b><br/>":"<td>")+listToString(f->getService(), "<br/>")+"</td>";
//            }
//  */
//            a += "<td></td>";
//    /*        a += "<td>Tf/Tb: "+f->getTimeTf().toString("hh:mm")+"<br/>";
//            if (f->getArt() != 3) a += "Zf/Zub/Service: "+f->getTimeZ().toString("hh:mm");
//            a += "</td>";
//      */
//            a += "<td>"+f->getWagenreihung()+(f->getWagenreihung() != "" ? "<br/>": "");
///*            if (f->getArt() <= 3 && f->getManager()->getGesamtzahl() > 0) {
//                a += QString::number(f->getManager()->getGesamtzahl());
//                a += (f->getManager()->getGesamtzahl() == 1 ? " reservierter Sitzplatz": " reservierte Sitzplätze");
//                a += "<br/>";
//            }
//  */          a += f->getBemerkungen()+"</td></tr>";

//        } else {
//            Activity *a = dynamic_cast<Activity*>(a);
//            // es ist ein Arbeitseinsatz

//        }
//    }


    a += "</tbody></table>";
    a += "</body></html>";
    d->setHtml(a);



    return print(pdf, paper, d);
}

bool Export::printPerson(Person *p, QPrinter *pdf, QPrinter *paper)
{
    QTextDocument *d = new QTextDocument();
    return print(pdf, paper, d);
}

bool Export::printPersonen(ManagerPersonal *m, QPrinter *pdf, QPrinter *paper)
{
    QTextDocument *d = new QTextDocument();
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
    QPrinter *p = new QPrinter(QPrinter::PrinterResolution);
    p->setOutputFormat(QPrinter::PdfFormat);

    QFileDialog f(parent, Qt::Dialog);

    p->setOutputFileName(f.getSaveFileName(parent, "Datei speichern unter...", QDir::currentPath()+"/"+path, "PDF-Dateien (*.pdf)"));
    if (p->outputFileName() == "") return nullptr;
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


void Export::preparePrinterSingle(QPrinter *p)
{
    if (p == nullptr) return;
    p->setFullPage(true);
    p->setPageMargins(20, 15, 20, 20, QPrinter::Millimeter);
    p->setPageOrientation(QPageLayout::Portrait);
}

void Export::preparePrinterList(QPrinter *p)
{
    if (p == nullptr) return;
    p->setPageMargins(15, 20, 15, 20, QPrinter::Millimeter);
    p->setPageOrientation(QPageLayout::Landscape);
}

