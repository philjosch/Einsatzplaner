#include "einstellungen.h"
#include "export.h"
#include "fileio.h"
#include "networking.h"

#include <QTemporaryFile>
#include <QPrintDialog>

const QString Export::DEFAULT_STYLESHEET = "body {float: none;} body, tr, th, td, p { font-size: 11px; font-weight: normal;}"
                            "table { border-width: 1px; border-style: solid; border-color: black; border-collapse: collapse;}"
                            "table th, table td { border-width: 1px; padding: 1px; border-style: solid; border-color: black; border-collapse: collapse;}"
                            "table tr, table td { page-break-inside: avoid; }"
                            "table tfoot tr, table tfoot td { border-width: 1px; }"
                            "ul { -qt-list-indent: 0; margin-top: 0px !important; margin-bottom: 0px !important }"
                            "li { text-indent: 6px; margin-top: 0px !important; margin-bottom: 0px !important; }"
                            "p.last { page-break-after: always; }";
const QFont Export::DEFAULT_FONT = QFont("Arial", 11, QFont::Normal);

bool Export::Aktivitaeten::printAktivitaetenEinzel(QList<AActivity *> liste, QPrinter *printer)
{
    if (liste.isEmpty()) return true;
    QString html = Manager::getHtmlFuerEinzelansichten(liste);
    return druckeHtmlAufDrucker(html, printer);
}

bool Export::Aktivitaeten::printAktivitaetenListe(QList<AActivity *> liste, QPrinter *printer)
{
    if (liste.isEmpty()) return true;
    QString html = Manager::getHtmlFuerListenansicht(liste) + zeitStempel();
    return druckeHtmlAufDrucker(html, printer);
}

bool Export::Aktivitaeten::printReservierung(Fahrtag *f, QPrinter *printer)
{
    if (f->getAnzahlReservierungen() == 0) return false;
    QString html = f->getHtmlFuerReservierungsuebersicht() + zeitStempel();
    return druckeHtmlAufDrucker(html, printer);
}

bool Export::Personal::printZeitenEinzelEinzel(Person *p, QPrinter *printer)
{
    if (p == nullptr) return false;
    QString html = p->getZeitenFuerEinzelAlsHTML() + zeitStempel();
    return druckeHtmlAufDrucker(html, printer);
}
bool Export::Personal::printZeitenEinzelListe(QList<Person *> liste, ManagerPersonal *m, Mitglied filter, QPrinter *printer)
{
    if (liste.isEmpty()) return false;
    QString html = m->getZeitenFuerEinzelListeAlsHTML(liste, filter);
    return druckeHtmlAufDrucker(html, printer);
}
bool Export::Personal::printZeitenListe(QList<Person *> personen, QSet<Category> data, Mitglied filter, QPrinter *printer)
{
    if (personen.isEmpty()) return true;
    QString html = ManagerPersonal::getZeitenFuerListeAlsHTML(personen, data, filter);
    return druckeHtmlAufDrucker(html, printer);
}

bool Export::Mitglieder::printMitgliederEinzelEinzel(Person *p, QPrinter *printer)
{
    if (p == nullptr) return false;
    QString html = p->getPersonaldatenFuerEinzelAlsHTML() + zeitStempel();
    return druckeHtmlAufDrucker(html, printer);
}
bool Export::Mitglieder::printMitgliederEinzelListe(QList<Person *> liste, ManagerPersonal *m, Mitglied filter, QPrinter *printer)
{
    if (liste.isEmpty()) return false;
    QString html = m->getMitgliederFuerEinzelListeAlsHTML(liste, filter);
    return druckeHtmlAufDrucker(html, printer);
}
bool Export::Mitglieder::printMitgliederListe(QList<Person*> liste, Mitglied filter, QPrinter *printer)
{
    if (liste.isEmpty()) return false;
    QString html = ManagerPersonal::getMitgliederFuerListeAlsHtml(liste, filter);
    return druckeHtmlAufDrucker(html, printer);
}

bool Export::Mitglieder::exportMitgliederAlsCSV(QList<Person *> liste, QString pfad)
{
    if (liste.isEmpty()) return false;
    return FileIO::saveToFile(pfad, ManagerPersonal::getMitgliederFuerListeAlsCSV(liste));
}

QPrinter *Export::getPrinterPaper(QWidget *parent, QPrinter::Orientation orientation)
{
    QPrinter *p = new QPrinter();
    preparePrinter(p, orientation);
    if (QPrintDialog(p, parent).exec() == QDialog::Accepted)
        return p;
    return nullptr;
}

QPrinter *Export::getPrinterPDF(QWidget *parent, QString path, QPrinter::Orientation orientation)
{
    QString filePath = FileIO::getFilePathSave(parent, path, QObject::tr("PDF-Dateien (*.pdf)"));
    if (filePath == "") return nullptr;
    QPrinter *p = new QPrinter(QPrinter::PrinterResolution);
    p->setOutputFormat(QPrinter::PdfFormat);
    p->setOutputFileName(filePath);
    preparePrinter(p, orientation);
    return p;
}

bool Export::Upload::uploadToServer(QList<AActivity *> liste, Networking::Server server)
{
    /* ERSTELLEN DER DATEI */
    QTemporaryFile tempFile;
    tempFile.open();
    QString localFile = tempFile.fileName();
    QPrinter *p = new QPrinter(QPrinter::PrinterResolution);
    p->setOutputFormat(QPrinter::PdfFormat);
    p->setOutputFileName(localFile);
    preparePrinter(p, QPrinter::Landscape);

    Aktivitaeten::printAktivitaetenListe(liste, p);

    return Networking::ladeDateiHoch(server, &tempFile);
}

int Export::Upload::autoUploadToServer(QList<AActivity*> liste, Networking::Server server)
{
    if (!Einstellungen::getUseAutoUpload()) return -1;

    if (Upload::uploadToServer(liste, server)) return 1;
    return 0;
}

bool Export::druckeHtmlAufDrucker(QString text, QPrinter *printer)
{
    if (printer == nullptr) return false;
    if (text == "") return false;
    QTextDocument *d = newDefaultDocument();
    d->setHtml(text);
    d->print(printer);
    return true;
}

void Export::preparePrinter(QPrinter *p, QPrinter::Orientation orientation)
{
    if (p == nullptr) return;
    p->setFullPage(true);
    switch (orientation) {
    case QPrinter::Portrait:
        p->setPageMargins(20, 15, 15, 15, QPrinter::Millimeter);
        p->setPageOrientation(QPageLayout::Portrait);
        break;
    case QPrinter::Landscape:
        p->setPageMargins(15, 20, 15, 15, QPrinter::Millimeter);
        p->setPageOrientation(QPageLayout::Landscape);
        break;
    }
}

QString Export::zeitStempel()
{
    return QObject::tr("<p><small>Erstellt am: %1</small></p>").arg(QDateTime::currentDateTime().toString(QObject::tr("d.M.yyyy HH:mm")));
}

QTextDocument *Export::newDefaultDocument()
{
    QTextDocument *d = new QTextDocument();
    d->setDefaultStyleSheet(DEFAULT_STYLESHEET);
    d->setDefaultFont(DEFAULT_FONT);
    d->setDocumentMargin(0);
    return d;
}

