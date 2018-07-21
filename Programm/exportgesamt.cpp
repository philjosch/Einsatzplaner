#include "exportgesamt.h"
#include "ui_exportgesamt.h"
#include "mainwindow.h"
#include "export.h"

#include <QPrinter>
#include <QPrintDialog>

ExportGesamt::ExportGesamt(Manager *m, QWidget *parent) : QDialog(parent), ui(new Ui::ExportGesamt)
{
    ui->setupUi(this);
    p = parent;
    ui->dateVon->setDate(QDate::currentDate());
    ui->dateBis->setDate(QDate::currentDate().addDays(60)); // Anzeige für zwei Monate in der Zukunft
    manager = m;

    hardReload();
}

ExportGesamt::~ExportGesamt()
{
    delete ui;
}

void ExportGesamt::reload()
{
    QListIterator<AActivity*> iter = manager->getActivities();
    int i = 0;
    while(iter.hasNext()) {
        AActivity *a = iter.next();
        if (actToList.contains(a)) {
            QString farbe = MainWindow::getFarbe(a);
            actToList.value(a)->setBackgroundColor(QColor(farbe));
            actToList.value(a)->setToolTip(a->getAnlass());
            i++;
            continue;
        }
        QString farbe = MainWindow::getFarbe(a);
        QListWidgetItem *item = new QListWidgetItem(a->getListString());
        item->setBackgroundColor(QColor(farbe));
        item->setToolTip(a->getAnlass());
        ui->listAnzeige->insertItem(i, item);
        liste.insert(i, a);
        actToList.insert(a, item);
        listToAct.insert(item, a);
        i++;
    }
    show();
}

void ExportGesamt::hardReload()
{
    QListIterator<AActivity*> iter = manager->getActivities();
    liste = QList<AActivity*>();
    actToList = QMap<AActivity*, QListWidgetItem*>();
    listToAct = QMap<QListWidgetItem*, AActivity*>();
    ui->listAnzeige->clear();

    while(iter.hasNext()) {
        AActivity *a = iter.next();
        QString farbe = MainWindow::getFarbe(a);
        QListWidgetItem *item = new QListWidgetItem(a->getListString());
        item->setBackgroundColor(QColor(farbe));
        item->setToolTip(a->getAnlass());
        ui->listAnzeige->insertItem(ui->listAnzeige->count(), item);
        liste.append(a);
        actToList.insert(a, item);
        listToAct.insert(item, a);
    }
    show();
}

void ExportGesamt::printPreview(QPrinter *printer)
{
    // Erstelle eine liste mit den Objekten für die Einzelansicht
    // Erstelle eine Liste für die Listenansicht
    QList<AActivity*> *listeListe = new QList<AActivity*>();
    for (AActivity *a: liste) {
        if(! actToList.value(a)->isHidden()) {
            listeListe->append(a);
        }
    }

    QTextDocument *d = new QTextDocument();
    QString b = "";
    b += "body, td, p { font-weight: normal !important;}";
    b += "table { border-width: 0.1em; border-style: solid; border-color: black; }";
    b += "table th, table td { border-width: 0.1em; padding: 0.1em; border-style: solid; border-color: black;}";
    b += "table tr, table td { page-break-inside: avoid; }";
    b += "ul { -qt-list-indent: 0; }";
    b += "li { text-indent: 1em; margin-top: 0px !important; margin-bottom: 0px !important; }";
//    b += "</style>";
    d->setDefaultStyleSheet(b);
    QFont myfont("Console", 11, QFont::Normal);
    d->setDefaultFont(myfont);
    d->setDocumentMargin(0);

    QString a = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"><html><head><title>Einsatzplan - Listenansicht</title></head><body>";
    a += "<h3>Übersicht über die Aktivitäten</h3>";
    a += "<table cellspacing='0' width='100%'><thead><tr> <th>Datum, Anlass</th> <th>Tf, Tb</th> <th><u>Zf</u>, Zub, <i>Begl.o.b.A</i></th> <th>Service</th> <th>Dienstzeiten</th> <th>Sonstiges</th></tr></thead><tbody>";
    for(AActivity *akt: *listeListe) {
        a += akt->getHtmlForTableView();
    }
    a += "</tbody></table>";
    a += "<p><small>Erstellt am: "+QDateTime::currentDateTime().toString("d.M.yyyy H:mm")+"</small></p>";
    a += "</body></html>";
    d->setHtml(a);
    d->print(printer);

}

void ExportGesamt::on_pushDrucken_clicked()
{
    // Erstelle eine liste mit den Objekten für die Einzelansicht
    QList<AActivity*> *listeEinzel = new QList<AActivity*>();
    // Erstelle eine Liste für die Listenansicht
    QList<AActivity*> *listeListe = new QList<AActivity*>();
    for (AActivity *a: liste) {
        if(! actToList.value(a)->isHidden()) {
            listeListe->append(a);
            if (actToList.value(a)->isSelected()) {
                listeEinzel->append(a);
            }
        }
    }

    // Lasse einene Drucker auswählen
    QPrinter *paperListe = nullptr;
    QPrinter *paperEinzel = nullptr;
    QPrinter *pdfListe = nullptr;
    QPrinter *pdfEinzel = nullptr;
    if (ui->checkListe->isChecked() && listeListe->length() > 0) {
        if (ui->checkPDF->isChecked())
            pdfListe = Export::getPrinterPDF(p, "Listenansicht.pdf");
        if (ui->checkAusdruck->isChecked())
            paperListe = Export::getPrinterPaper(p);
        Export::printList(listeListe, pdfListe, paperListe);
    }
    if (ui->checkEinzel->isChecked() && listeEinzel->length() > 0) {
        if (ui->checkPDF->isChecked())
            pdfEinzel = Export::getPrinterPDF(p, "Einzelansicht.pdf");
        if (ui->checkAusdruck->isChecked())
            paperEinzel = Export::getPrinterPaper(p);
        Export::printSingle(listeEinzel, pdfEinzel, paperEinzel);
    }
}

void ExportGesamt::on_comboVon_currentIndexChanged(int index)
{
    switch (index) {
    case 0: // Ab datum
        ui->dateVon->setEnabled(true);
        break;
    case 1: // Ab heute
    case 2: // Ab beginn des Jahres
    case 3: // Ab egal
        ui->dateVon->setEnabled(false);
        break;
    default:
        ui->dateVon->setEnabled(true);
    }
    show();
}

void ExportGesamt::on_dateVon_dateChanged(const QDate &date)
{
    show();
}

void ExportGesamt::on_comboBis_currentIndexChanged(int index)
{
    switch (index) {
    case 0: // Bis datum
        ui->dateBis->setEnabled(true);
        break;
    case 1: // Bis heute
    case 2: // Bis Ende das Jahres
    case 3: // Bis egal egal
        ui->dateBis->setEnabled(false);
        break;
    default:
        ui->dateBis->setEnabled(true);
    }
    show();
}

void ExportGesamt::on_dateBis_dateChanged(const QDate &date)
{
    show();
}

void ExportGesamt::on_comboFahrtag_currentIndexChanged(int index)
{
    show();
}

void ExportGesamt::on_checkActivity_clicked(bool checked)
{
    show();
}

void ExportGesamt::show()
{
    for(AActivity *a: liste) {
        actToList.value(a)->setHidden(!testShow(a));
    }
}

bool ExportGesamt::testShow(AActivity *a)
{
    // Prüfen bei den Einträgen, ob das Datum stimmt
    switch (ui->comboVon->currentIndex()) {
    case 0: // Ab datum
        if (a->getDatum() < ui->dateVon->date())
            return false;
        break;
    case 1: // Ab heute
        if (a->getDatum() < QDate::currentDate())
            return false;
        break;
    case 2: // Ab beginn des Jahres
        if (a->getDatum().year() < QDate::currentDate().year())
            return false;
        break;
    case 3: // Ab egal
        break;
    default:
        break;
    }

    switch (ui->comboBis->currentIndex()) {
    case 0: // Bis datum
        if (a->getDatum() > ui->dateBis->date())
            return false;
        break;
    case 1: // Bis heute
        if (a->getDatum() > QDate::currentDate())
            return false;
        break;
    case 2: // Bis Ende des Jahres
        if (a->getDatum().year() > QDate::currentDate().year())
            return false;
        break;
    case 3: // Bis egal
        break;
    default:
        break;
    }

    // Prüfen, ob die Art stimmt (Fahrtag, Arbeitseinsatz)
    if (Fahrtag *f = dynamic_cast<Fahrtag*>(a)) {
        int i = ui->comboFahrtag->currentIndex();
        if (i == 9) return false;
        if (i != 8) {
            if (i != f->getArt()) {
                return false;
            }
        }
        return true;
        // es ist ein fahrtag
    } else {
        return ui->checkActivity->isChecked();
        // es ist kein fahrtag
    }
}

void ExportGesamt::on_pushButton_clicked()
{
    hardReload();
}
