#include "einstellungen.h"
#include "export.h"
#include "fileio.h"
#include "networking.h"

#include <QTemporaryFile>
#include <QPrintDialog>

const QString Export::DEFAULT_STYLESHEET = "body {float: none;} body, tr, th, td, p { font-size: 10px; font-weight: normal !important;}"
                            "table { border-width: 1px; border-style: solid; border-color: black; border-collapse: collapse;}"
                            "table th, table td { border-width: 1px; padding: 1px; border-style: solid; border-color: black; border-collapse: collapse;}"
                            "table tr, table td { page-break-inside: avoid; }"
                            "table tfoot tr, table tfoot td { border-width: 1px; }"
                            "ul { -qt-list-indent: 0; margin-top: 0px !important; margin-bottom: 0px !important }"
                            "li { text-indent: 6px; margin-top: 0px !important; margin-bottom: 0px !important; }"
                            "p.last { page-break-after: always; }";
const QFont Export::DEFAULT_FONT = QFont("Arial", 11, QFont::Normal);

bool Export::printAktivitaetenEinzel(QList<AActivity *> liste, QPrinter *printer)
{
    if (printer == nullptr) return false;
    if (liste.isEmpty()) return true;
    QTextDocument *d = newDefaultDocument();
    QString html = Manager::getHtmlFuerEinzelansichten(liste);
    d->setHtml(html);
    d->print(printer);
    return true;
}

bool Export::printAktivitaetenListe(QList<AActivity *> liste, QPrinter *printer)
{
    if (printer == nullptr) return false;
    if (liste.isEmpty()) return true;
    QTextDocument *d = newDefaultDocument();
    QString a = Manager::getHtmlFuerListenansicht(liste);
    a += QObject::tr("<p><small>Erstellt am: %1</small></p>").arg(QDateTime::currentDateTime().toString("d.M.yyyy HH:mm"));
    d->setHtml(a);
    d->print(printer);
    return true;
}

bool Export::printReservierung(Fahrtag *f, QPrinter *printer)
{
    if (f->getAnzahlReservierungen() == 0 || printer == nullptr) return false;
    QTextDocument *d = newDefaultDocument();
    QString a = f->getHtmlFuerReservierungsuebersicht();
    a += QObject::tr("<p><small>Erstellt am: %1</small></p>").arg(QDateTime::currentDateTime().toString("d.M.yyyy HH:mm"));
    d->setHtml(a);
    d->print(printer);
    return true;
}

bool Export::printZeitenEinzelEinzel(Person *p, QPrinter *printer)
{
    if (p == nullptr || printer == nullptr) return false;
    QTextDocument *d = newDefaultDocument();
    QString a = p->getZeitenFuerEinzelAlsHTML();
    a += QObject::tr("<p><small>Erstellt am: %1</small></p>").arg(QDateTime::currentDateTime().toString("d.M.yyyy HH:mm"));
    d->setHtml(a);
    d->print(printer);
    return true;
}
bool Export::printZeitenEinzelListe(QList<Person *> liste, ManagerPersonal *m, Mitglied filter, QPrinter *printer)
{
    if (printer == nullptr) return false;
    QTextDocument *d = newDefaultDocument();
    QString a = m->getZeitenFuerEinzelListeAlsHTML(liste, filter);
    d->setHtml(a);
    d->print(printer);
    return true;
}
bool Export::printZeitenListe(QList<Person *> personen, QSet<Category> data, Mitglied filter, QPrinter *printer)
{
    if (printer == nullptr) return false;
    if (personen.isEmpty()) return true;
    QTextDocument *d = newDefaultDocument();
    QString a = ManagerPersonal::getZeitenFuerListeAlsHTML(personen, data, filter);
    d->setHtml(a);
    d->print(printer);
    return true;
}

bool Export::printMitgliederEinzelEinzel(Person *p, QPrinter *printer)
{
    if (p == nullptr || printer == nullptr) return false;
    QTextDocument *d = newDefaultDocument();
    QString a = p->getPersonaldatenFuerEinzelAlsHTML();
    a += QObject::tr("<p><small>Erstellt am: %1</small></p>").arg(QDateTime::currentDateTime().toString("d.M.yyyy HH:mm"));
    d->setHtml(a);
    d->print(printer);
    return true;
}
bool Export::printMitgliederEinzelListe(QList<Person *> liste, ManagerPersonal *m, Mitglied filter, QPrinter *printer)
{
    if (printer == nullptr) return false;
    QTextDocument *d = newDefaultDocument();
    QString a = m->getMitgliederFuerEinzelListeAlsHTML(liste, filter);
    d->setHtml(a);
    d->print(printer);
    return true;
}
bool Export::printMitgliederListe(QList<Person*> liste, Mitglied filter, QPrinter *printer)
{
    if (printer == nullptr) return false;
    QTextDocument *d = newDefaultDocument();
    QString a = ManagerPersonal::getMitgliederFuerListeAlsHtml(liste, filter);
    d->setHtml(a);
    d->print(printer);
    return true;
}

bool Export::exportMitgliederAlsCSV(QList<Person *> liste, Mitglied filter, QString pfad)
{
    return FileIO::saveToFile(pfad, ManagerPersonal::getMitgliederFuerListeAlsCSV(liste, filter));
}

QPrinter *Export::getPrinterPaper(QWidget *parent, QPrinter::Orientation orientation)
{
    QPrinter *p = new QPrinter();
    if (orientation == QPrinter::Orientation::Portrait)
        preparePrinterPortrait(p);
    else
        preparePrinterLandscape(p);
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
    if (orientation == QPrinter::Orientation::Portrait)
        preparePrinterPortrait(p);
    else
        preparePrinterLandscape(p);
    return p;
}

bool Export::testServerConnection(ManagerFileSettings *settings)
{
    QString request = settings->getFullServer()+"?id="+settings->getId();
    return (Networking::ladeDatenVonURL(request) == "OK");
}

bool Export::uploadToServer(ManagerFileSettings *settings, QList<AActivity *> liste)
{
    if (! settings->getEnabled()) return true;

    /* ERSTELLEN DER DATEI */
    QTemporaryFile tempFile;
    tempFile.open();
    QString localFile = tempFile.fileName();
    QPrinter *p = new QPrinter(QPrinter::PrinterResolution);
    p->setOrientation(QPrinter::Orientation::Landscape);
    p->setOutputFormat(QPrinter::PdfFormat);
    p->setOutputFileName(localFile);

    printAktivitaetenListe(liste, p);

    return Networking::ladeDateiHoch(settings->getFullServer(), &tempFile, settings->getId());
}

int Export::autoUploadToServer(ManagerFileSettings *settings, Manager *mgr)
{
    /* EINSTELLUNGEN LESEN */
    if (!Einstellungen::getUseAutoUpload()) return -1;
    if (!settings->getAutom()) return -1;

    /* LISTE MIT DEN AKTIVITAETEN ERSTELLEN */
    QList<AActivity *> liste = QList<AActivity*>();
    QListIterator<AActivity*> iter = mgr->getActivities();
    while(iter.hasNext()) {
        AActivity *a = iter.next();
        // Beginndatum
        if (settings->getStartdate() == "tdy") {
            if (a->liegtInVergangenheit()) {
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
            if (a->getArt() == Art::Arbeitseinsatz)
                continue;
        }
        liste.append(a);
    }

    /* UPLOADFUNKTION NUTZEN; UM DATEIEN HOCHZULADEN */
    if (liste.length() == 0)
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

QTextDocument *Export::newDefaultDocument()
{
    QTextDocument *d = new QTextDocument();
    d->setDefaultStyleSheet(DEFAULT_STYLESHEET);
    d->setDefaultFont(DEFAULT_FONT);
    d->setDocumentMargin(0);
    return d;
}

