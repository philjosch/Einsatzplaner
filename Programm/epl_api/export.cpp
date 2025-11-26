#include "export.h"
#include "fileio.h"
#include "networking.h"
#include "eplexception.h"

#include <QTemporaryFile>
#include <QPrintDialog>

using namespace EplException;

const QString Export::DEFAULT_STYLESHEET = "body {float: none;} body, tr, th, td, p { font-size: 11px; font-weight: normal;}"
                            "table { border-width: 1px; border-style: solid; border-color: black; border-collapse: collapse;}"
                            "table th, table td { border-width: 1px; padding: 1px; border-style: solid; border-color: black; border-collapse: collapse;}"
                            "table tr, table td { page-break-inside: avoid; }"
                            "table tfoot tr, table tfoot td { border-width: 1px; }"
                            "ul { -qt-list-indent: 0; margin-top: 0px !important; margin-bottom: 0px !important }"
                            "li { text-indent: 6px; margin-top: 0px !important; margin-bottom: 0px !important; }"
                            "p.break { page-break-after: always; }";
const QFont Export::DEFAULT_FONT = QFont("Arial", 11, QFont::Normal);

Export *Export::getPrinterPaper(QWidget *parent, QPageLayout::Orientation orientation)
{
    QPrinter *p = new QPrinter();
    preparePrinter(p, orientation);
    if (QPrintDialog(p, parent).exec() == QDialog::Accepted)
        return new Export(p);
    return nullptr;
}

Export *Export::getPrinterPDF(QWidget *parent, QString fileName, QPageLayout::Orientation orientation)
{
    QString filePath = FileIO::getFilePathSave(parent, fileName, FileIO::DateiTyp::PDF);
    if (filePath == "") return nullptr;
    QPrinter *p = new QPrinter(QPrinter::PrinterResolution);
    p->setOutputFormat(QPrinter::PdfFormat);
    p->setOutputFileName(filePath);
    preparePrinter(p, orientation);
    return new Export(p);
}

ExportUpload *Export::getPrinterOnline(Networking::Server server, QPageLayout::Orientation orientation)
{
    QPrinter *p = new QPrinter(QPrinter::PrinterResolution);
    preparePrinter(p, orientation);
    return new ExportUpload(p, server);
}

Export::Export(QPrinter *printer)
{
    this->printer = printer;
}

bool Export::exportHTML(QString htmlString)
{
    if (printer == nullptr) return false;
    QTextDocument *d = newDefaultDocument();
    d->setHtml(htmlString);
    d->print(printer);
    return true;
}

void Export::preparePrinter(QPrinter *p, QPageLayout::Orientation orientation)
{
    if (p == nullptr) return;
    p->setFullPage(true);
    switch (orientation) {
    case QPageLayout::Orientation::Portrait:
        p->setPageMargins(QMarginsF(20, 15, 15, 15), QPageLayout::Millimeter);
        break;
    case QPageLayout::Orientation::Landscape:
        p->setPageMargins(QMarginsF(15, 20, 15, 15), QPageLayout::Millimeter);
        break;
    }
    p->setPageOrientation(orientation);
}

QString Export::zeitStempel(bool seitenUmbruch)
{
    QString s = QObject::tr("Erstellt am: %1").arg(QDateTime::currentDateTime().toString(QObject::tr("dd.MM.yyyy HH:mm")));
    if (seitenUmbruch) {
        return "<p class='break'><small>"+s+"</small></p>";
     } else {
        return "<p><small>"+s+"</small></p>";
    }
}

QTextDocument *Export::newDefaultDocument()
{
    QTextDocument *d = new QTextDocument();
    d->setDefaultStyleSheet(DEFAULT_STYLESHEET);
    d->setDefaultFont(DEFAULT_FONT);
    d->setDocumentMargin(0);
    return d;
}

ExportUpload::ExportUpload(QPrinter *printer, Networking::Server server) : Export(printer)
{
    this->server = server;
    printer->setOutputFormat(QPrinter::PdfFormat);
}

bool ExportUpload::exportHTML(QString htmlString)
{
    if (printer == nullptr) return false;

    QTemporaryFile tempFile;
    if (! tempFile.open()) {
        throw FileWriteException();
    }
    QString localFile = tempFile.fileName();
    printer->setOutputFileName(localFile);

    QTextDocument *d = newDefaultDocument();
    d->setHtml(htmlString);
    d->print(printer);

    bool status = Networking::ladeDateiHoch(server, &tempFile);
    tempFile.close();
    return status;
}
