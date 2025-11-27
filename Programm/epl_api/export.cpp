#include "export.h"
#include "fileio.h"
#include "networking.h"

#include <QTemporaryFile>
#include <QPrintDialog>
#include <QPdfWriter>

const QString ExportHtml::DEFAULT_STYLESHEET =
                            "body {float: none;} body, tr, th, td, p { font-size: 11pt; font-weight: normal;}"
                            "table { border-width: 1px; border-style: solid; border-color: black; border-collapse: collapse;}"
                            "table th, table td { border-width: 1px; padding: 1px; border-style: solid; border-color: black; border-collapse: collapse;}"
                            "table tr, table td { page-break-inside: avoid; }"
                            "table tfoot tr, table tfoot td { border-width: 1px; }"
                            "ul { -qt-list-indent: 0; margin-top: 0px !important; margin-bottom: 0px !important }"
                            "li { margin-left: 6px; margin-top: 0px !important; margin-bottom: 0px !important; }"
                            "p.break { page-break-after: always; }";
const QFont ExportHtml::DEFAULT_FONT = QFont("Arial", 11, QFont::Normal);


void Export::prepareLayout(QPagedPaintDevice *device, QPageLayout::Orientation orientation)
{
    if (device == nullptr) return;
    QPageLayout l = device->pageLayout();
    l.setMode(QPageLayout::Mode::FullPageMode);
    l.setUnits(QPageLayout::Millimeter);
    l.setOrientation(orientation);
    switch (orientation) {
    case QPageLayout::Orientation::Portrait:
        l.setMargins(QMarginsF(20, 15, 15, 15));
        break;
    case QPageLayout::Orientation::Landscape:
        l.setMargins(QMarginsF(15, 20, 15, 15));
        break;
    }
    device->setPageLayout(l);
}

QPrinter *Export::generatePrinter(QWidget *parent, QPageLayout::Orientation orientation)
{
    QPrinter *p = new QPrinter();
    prepareLayout(p, orientation);
    if (QPrintDialog(p, parent).exec() == QDialog::Accepted)
        return p;
    return nullptr;
}

ExportHtml::ExportHtml()
{
    format = Export::HTML;
}
QString ExportHtml::timeStamp(bool pageBreak)
{
    QString s = QObject::tr("Erstellt am: %1").arg(QDateTime::currentDateTime().toString(QObject::tr("dd.MM.yyyy HH:mm")));
    if (pageBreak) {
        return "<p class='break'><small>"+s+"</small></p>";
    } else {
        return "<p><small>"+s+"</small></p>";
    }
}
QTextDocument *ExportHtml::newDefaultDocument()
{
    QTextDocument *d = new QTextDocument();
    d->setDefaultStyleSheet(DEFAULT_STYLESHEET);
    d->setDefaultFont(DEFAULT_FONT);
    d->setDocumentMargin(0);
    return d;
}


ExportHtmlPaper::ExportHtmlPaper(QPrinter *printer) : ExportHtml()
{
    this->printer = printer;
}
ExportHtmlPaper *ExportHtmlPaper::generate(QWidget *parent, QPageLayout::Orientation orientation)
{
    QPrinter *p = generatePrinter(parent, orientation);
    if (p == nullptr)
        return nullptr;
    return new ExportHtmlPaper(p);
}
bool ExportHtmlPaper::exportData(QString data)
{
    if (printer == nullptr) return false;
    QTextDocument *d = newDefaultDocument();
    d->setHtml(data);
    d->print(printer);
    return true;
}


ExportHtmlPdf::ExportHtmlPdf(QString filePath, QPageLayout::Orientation orientation) : ExportHtml()
{
    if (filePath == "") {
        printer = nullptr;
        return;
    }
    printer = new QPdfWriter(filePath);
    Export::prepareLayout(printer, orientation);
}
ExportHtmlPdf *ExportHtmlPdf::generate(QWidget *parent, QString fileName, QPageLayout::Orientation orientation)
{
    QString filePath = FileIO::getFilePathSave(parent, fileName, FileIO::DateiTyp::PDF);
    if (filePath == "") return nullptr;
    return new ExportHtmlPdf(filePath, orientation);
}
bool ExportHtmlPdf::exportData(QString data)
{
    if (printer == nullptr) return false;
    QTextDocument *d = newDefaultDocument();
    d->setDefaultStyleSheet(d->defaultStyleSheet() + "li { margin-left: 120px; }" );
    d->setHtml(data);
    d->print(printer);
    return true;
}


ExportHtmlUpload::ExportHtmlUpload(Networking::Server server, QPageLayout::Orientation orientation) : ExportHtml()
{
    this->server = server;
    tempFile = new QTemporaryFile();
    if (! tempFile->open()) {
        printer = nullptr;
        return;
    }
    printer = new ExportHtmlPdf(tempFile->fileName(), orientation);
}
ExportHtmlUpload *ExportHtmlUpload::generate(Networking::Server server, QPageLayout::Orientation orientation)
{
    return new ExportHtmlUpload(server, orientation);
}
bool ExportHtmlUpload::exportData(QString data)
{
    if (printer == nullptr) return false;
    printer->exportData(data);
    return Networking::ladeDateiHoch(server, tempFile);
}


ExportCsv::ExportCsv()
{
    format = Export::CSV;
}

ExportCsvFile::ExportCsvFile(QString path) : ExportCsv()
{
    filePath = path;
}
ExportCsvFile *ExportCsvFile::generate(QWidget *parent, QString fileName)
{
    QString filePath = FileIO::getFilePathSave(parent, fileName, FileIO::DateiTyp::CSV);
    if (filePath == "") return nullptr;
    return new ExportCsvFile(filePath);
}
bool ExportCsvFile::exportData(QString data)
{
    return FileIO::saveToFile(filePath, data);
}
