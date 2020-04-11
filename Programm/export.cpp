#include "export.h"
#include "fileio.h"

#include <QEventLoop>
#include <QNetworkReply>
#include <QPrintDialog>
#include <QNetworkAccessManager>
#include <QTemporaryFile>
#include <QSettings>

const QString Export::DEFAULT_STYLESHEET = "body {float: none;} body, tr, th, td, p { font-size: 10px; font-weight: normal !important;}"
                            "table { border-width: 1px; border-style: solid; border-color: black; border-collapse: collapse;}"
                            "table th, table td { border-width: 1px; padding: 1px; border-style: solid; border-color: black; border-collapse: collapse;}"
                            "table tr, table td { page-break-inside: avoid; }"
                            "table tfoot tr, table tfoot td { border-width: 1px; }"
                            "ul { -qt-list-indent: 0; margin-top: 0px !important; margin-bottom: 0px !important }"
                            "li { text-indent: 12px; margin-top: 0px !important; margin-bottom: 0px !important; }"
                            "p.last { page-break-after: always; }";
const QFont Export::DEFAULT_FONT = QFont("Arial", 11, QFont::Normal);

bool Export::printEinzelansichten(QList<AActivity *> liste, QPrinter *printer)
{
    if (printer == nullptr) return false;
    QTextDocument *d = newDefaultDocument();
    QString html = Manager::getHtmlFuerEinzelansichten(liste);
    d->setHtml(html);
    d->print(printer);
    return true;
}

bool Export::printList(QList<AActivity *> liste, QPrinter *printer)
{
    if (printer == nullptr) return false;
    QTextDocument *d = newDefaultDocument();
    QString a = Manager::getHtmlFuerListenansicht(liste);
    a += "<p><small>Erstellt am: "+QDateTime::currentDateTime().toString("d.M.yyyy H:mm")+"</small></p>";
    d->setHtml(a);
    d->print(printer);
    return true;
}

bool Export::printReservierung(Fahrtag *f, QPrinter *printer)
{
    if (f->getAnzahl() == 0 || printer == nullptr) return false;
    QTextDocument *d = newDefaultDocument();
    QString a = f->getHtmlFuerReservierungsuebersicht();
    a += "<p><small>Erstellt am: "+QDateTime::currentDateTime().toString("d.M.yyyy H:mm")+"</small></p>";
    d->setHtml(a);
    d->print(printer);
    return true;
}

bool Export::printPerson(ManagerPersonal *m, QPrinter *printer)
{
    if (printer == nullptr) return false;
    QTextDocument *d = newDefaultDocument();
    QString a = m->getHtmlFuerEinzelansicht();
    d->setHtml(a);
    d->print(printer);
    return true;
}

bool Export::printPerson(Person *p, QPrinter *printer)
{
    if (p == nullptr || printer == nullptr) return false;
    QTextDocument *d = newDefaultDocument();
    QString a = p->getHtmlForDetailPage();
    a += "<p><small>Erstellt am: "+QDateTime::currentDateTime().toString("d.M.yyyy HH:mm")+"</small></p>";
    d->setHtml(a);
    d->print(printer);
    return true;
}

bool Export::printPersonenGesamtuebersicht(QList<Person *> personen, QSet<Category> data, QPrinter *printer)
{
    if (printer == nullptr) return false;
    QTextDocument *d = newDefaultDocument();
    QString a = ManagerPersonal::getHtmlFuerGesamtuebersicht(personen, data);
    a += "<p><small>Erstellt am: "+QDateTime::currentDateTime().toString("d.M.yyyy HH:mm")+"</small></p>";
    d->setHtml(a);
    d->print(printer);
    return true;
}

bool Export::printMitglieder(ManagerPersonal *m, QPrinter *printer)
{
    if (printer == nullptr) return false;
    QTextDocument *d = newDefaultDocument();
    QString a = m->getHtmlFuerMitgliederliste();
    a += "<p><small>Erstellt am: "+QDateTime::currentDateTime().toString("d.M.yyyy HH:mm")+"</small></p>";
    d->setHtml(a);
    d->print(printer);
    return true;
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

bool Export::uploadToServer(ManagerFileSettings *settings, QList<AActivity *> liste)
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
    QList<AActivity *> liste = QList<AActivity*>();
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

