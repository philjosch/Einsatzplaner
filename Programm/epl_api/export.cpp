#include "export.h"
#include "fileio.h"
#include "networking.h"
#include "eplexception.h"
#include "manager.h"

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

QPrinter *Export::getPrinterPaper(QWidget *parent, QPageLayout::Orientation orientation)
{
    QPrinter *p = new QPrinter();
    preparePrinter(p, orientation);
    if (QPrintDialog(p, parent).exec() == QDialog::Accepted)
        return p;
    return nullptr;
}

QPrinter *Export::getPrinterPDF(QWidget *parent, QString path, QPageLayout::Orientation orientation)
{
    QString filePath = FileIO::getFilePathSave(parent, path, FileIO::DateiTyp::PDF);
    if (filePath == "") return nullptr;
    QPrinter *p = new QPrinter(QPrinter::PrinterResolution);
    p->setOutputFormat(QPrinter::PdfFormat);
    p->setOutputFileName(filePath);
    preparePrinter(p, orientation);
    return p;
}

void Export::uploadToServer(QList<AActivity *> liste, Networking::Server server)
{
    /* ERSTELLEN DER DATEI */
    QTemporaryFile tempFile;
    tempFile.open();
    QString localFile = tempFile.fileName();
    QPrinter *p = new QPrinter(QPrinter::PrinterResolution);
    p->setOutputFormat(QPrinter::PdfFormat);
    p->setOutputFileName(localFile);
    preparePrinter(p, QPageLayout::Orientation::Landscape);

    Manager::printListenansicht(liste, p);

    if (! Networking::ladeDateiHoch(server, &tempFile)) {
        throw NetworkingException();
    }
}

bool Export::druckeHtml(QString text, QPrinter *printer)
{
    if (printer == nullptr) return false;
    QTextDocument *d = newDefaultDocument();
    d->setHtml(text);
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

