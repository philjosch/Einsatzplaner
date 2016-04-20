#include "export.h"
#include "ui_export.h"
#include <QPrinter>
#include <QPrintDialog>
#include <QList>
#include <QColor>
#include "planerfahrtage.h"
#include <QFileDialog>

Export::Export(ManagerZuege *manager, QWidget *parent) : QDialog(parent), ui(new Ui::Export) {
    ui->setupUi(this);
    this->manager = manager;
    map = new QMap<QListWidgetItem*, Fahrtag*>();
    ui->dateAb->setDate(QDate::currentDate());
    ui->dateBis->setDate(QDate::currentDate().addMonths(1));

    for (int i = 0; i < manager->getFahrtage()->length(); i++) {
        Fahrtag *f = manager->getFahrtage()->at(i);

        QListWidgetItem *item = new QListWidgetItem(f->getDatum().toString("dddd. dd.M.yyyy"));
        item->setToolTip(f->getAnlass());
        item->setBackgroundColor(PlanerFahrtage::getFarbe(f->getArt()));
        ui->listZuege->insertItem(i, item);
        map->insert(item, f);
    }
}

Export::~Export()
{
    delete ui;
}

void Export::on_comboAb_currentIndexChanged(int index) {
    ui->dateAb->setEnabled(index == 1);
    update();
}

void Export::on_comboBis_currentIndexChanged(int index) {
    ui->dateBis->setEnabled(index == 1);
    update();
}

void Export::on_ComboArt_currentIndexChanged(int index) {
    update();
}
void Export::on_dateBis_dateChanged(const QDate &date) {
    update();
}
void Export::on_dateAb_dateChanged(const QDate &date) {
    update();
}

void Export::update() {
    ui->listZuege->clearSelection();
    for (int i = 0; i < map->keys().count(); i++) {
        QListWidgetItem *item = map->keys().at(i);
        Fahrtag *f = map->value(item, new Fahrtag(new QListWidgetItem(), new QListWidget()));

        // Testen, ob Ab-Datum stimmt
        bool ab = false;
        switch (ui->comboAb->currentIndex()) {
        case 0:
            ab = f->getDatum() >= QDate::currentDate();
            break;
        case 1:
            ab = f->getDatum() >= ui->dateAb->date();
            break;
        case 2:
            ab = f->getDatum().year() == QDate::currentDate().year();
            break;
        default:
            ab = true;
            break;
        }

        // Testen, ob bis datum stimmt
        bool bis = false;
        switch (ui->comboBis->currentIndex()) {
        case 0:
            bis = f->getDatum() <= QDate::currentDate();
            break;
        case 1:
            bis = f->getDatum() <= ui->dateBis->date();
            break;
        case 2:
            bis = f->getDatum().year() == QDate::currentDate().year();
            break;
        default:
            bis = true;
            break;
        }

        // Testen, ob Art stimmet
        int index = ui->ComboArt->currentIndex();
        bool art = (f->getArt() == index) || (index == 8);

        item->setHidden(! (ab && bis && art));
    }
}

void Export::on_buttonDrucken_clicked()
{
    QPrinter printer;
    if (ui->checkSingle->isChecked()) {
        printer.setPageOrientation(QPageLayout::Portrait);
        printer.setPageMargins(20, 15, 20, 20, QPrinter::Millimeter);

        QPrintDialog *dlg = new QPrintDialog(&printer, this);
        if (dlg->exec() == QDialog::Accepted) {
            QTextDocument *document = createDocSingle(&printer);
            document->print(&printer);
        }
    }

    if (ui->checkListe->isChecked()) {
        printer.setPageOrientation(QPageLayout::Landscape);
        printer.setPageMargins(15, 20, 15, 20, QPrinter::Millimeter);

        QPrintDialog *dlg = new QPrintDialog(&printer, this);
        if (dlg->exec() == QDialog::Accepted) {
            QTextDocument *document = createDocListe(&printer);
            document->print(&printer);
        }
    }
}

void Export::on_buttonPDF_clicked()
{
    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageMargins(20, 15, 20, 20, QPrinter::Millimeter);
    QString path;
    if (ui->checkSingle->isChecked()) {
        printer.setPageOrientation(QPageLayout::Portrait);
        QTextDocument *document = createDocSingle(&printer);
        path = QFileDialog::getSaveFileName(this, "Datei speichern unter...", QDir::homePath()+"/Einzelansichten.pdf", "PDF-Dateien (*.pdf)");
        if (path != "") {
            printer.setOutputFileName(path);
            document->print(&printer);
        }
    }
    if (ui->checkListe->isChecked()) {
        printer.setPageMargins(15, 20, 15, 20, QPrinter::Millimeter);
        printer.setPageOrientation(QPageLayout::Landscape);
        QTextDocument *document = createDocListe(&printer);
        path = QFileDialog::getSaveFileName(this, "Datei speichern unter...", QDir::homePath()+"/Listenansicht.pdf", "PDF-Dateien (*.pdf)");
        if (path != "") {
            printer.setOutputFileName(path);
            document->print(&printer);
        }
    }
}

QTextDocument *Export::createDocSingle(QPrinter *printer)
{
    QTextDocument *doc = new QTextDocument(this);
    doc->setDocumentMargin(0);
    doc->setPageSize(QSizeF(printer->pageRect().size()));
    QString a = "<html><head><title>Einsatzplan</title></head><body>";
    a += "<style type=\"text/css\">";
    a += "body{float: none} body,td,th,p { font-size: 80%;}";
    a += "table {border-width: 1px; border-style: solid; border-color: black; border-collapse: collapse;} h1.pb { page-break-before: always; }";
    a +=" table th, table td { border-width: 1px; border-collapse: collapse; padding: 0px; border-style: solid; border-color: black; }";
    a+="</style>";
    bool first = false;
    for(int i = 0; i < manager->getFahrtage()->length(); i++) {
        Fahrtag *f = manager->getFahrtage()->at(i);
        if (isAllowedSingle(f)) {
            a += (first?"<h1 class='pb'>":"<h1>")+PlanerFahrtage::getArt(f->getArt())+" am "+f->getDatum().toString("d.M.yyyy")+(f->getWichtig()?" WICHTIG!":"")+"</h1>";
            first = true;
            if (f->getAnlass()!= "") {
                a += "<p><b>Anlass:</b><br/>"+f->getAnlass()+"</p>";
            }
            a += "<p><b>Wagenreihung:</b> "+f->getWagenreihung()+"</p>";
            a += "<p><b>Dienstbeginn</b>:<br/>Tf, Tb: "+f->getTimeTf().toString("hh:mm")+"<br/>Zf/Zub/Service: "+f->getTimeZ().toString("hh:mm")+"</p>";

            a += "<p><b>Triebfahrzeugführer (Tf), Triebfahrzeugbegleiter(Tb)";
            a += (f->getBenoetigeTf() ?" werden benötigt": "");
            a += ":</b><br/>"+listToString(f->getTf(), " | ")+"</p>";

            if (f->getArt() != 3) {
                a += "<p><b>Zugführer";
                a += (f->getBenoetigeZf() ?" wird benötigt":"");
                a += ":</b><br/>"+listToString(f->getZf(), " | ")+"</p>";

                a += "<p><b>Zugbegleiter und Begleiter ohne besondere Ausbildung";
                a += (f->getBenoetigeZub() ?" werden benötigt":"");
                a += ":</b><br/>"+listToString(f->getZub(), " | ")+"</p>";

                a += "<p><b>Service-Personal";
                a += (f->getBenoetigeService() ?" wird benötigt":"");
                a += ":</b><br/>"+listToString(f->getService(), " | ") +"</p>";
            }
            if (f->getBemerkungen()!= "") {
                a += "<p>Bemerkungen:<br/>"+f->getBemerkungen()+"</p>";
            }
            a += "<p><b>Reservierungen:</b><br/>Bereits "+QString::number(f->getManager()->getGesamtzahl());
            a += (f->getManager()->getGesamtzahl() == 1 ? " Platz " :  " Plätze ");
            a += "belegt.</p>";

            if (f->getManager()->getReservierungen()->length() > 0) {
                a +="<table><thead><tr><th>Kontakt</th><th>Sitzplätze</th><th>Ein- und Ausstieg</th><th>Sonstiges</th></tr></thead><tbody>";
                for (int j = 0; j < f->getManager()->getReservierungen()->length(); j++) {
                    a += stringFromReservierung(f->getManager()->getReservierungen()->at(j));
                }
                a += "</tbody></table>";
            }
            a += "<p class='last'><small>Erstellt am: "+QDate::currentDate().toString("d.M.yyyy")+"</small></p>";
        }
    }
    a += "</body></html>";
    doc->setHtml(a);

    return doc;
}

QString Export::stringFromReservierung(Reservierung *res)
{
    QString a = "<tr><td>"+res->getName()+"<br/>"+res->getTelefon()+"<br/>"+res->getMail()+"</td>";
    a += "<td>"+QString::number(res->getAnzahl())+" Plätze in "+res->getKlasse()+"<br/>"+res->getSitzplatz()+"</td>";
    a += "<td>"+res->getStartZug().at(0)+" "+res->getStartHp().at(0)+"<br/>->"+res->getEndeZug().at(0)+" "+res->getEndeHp().at(0)+"</td>";
    a += (res->getFahrrad() ? "<td>Fahrrad!<br/>" : "<td>")+res->getSonstiges()+"</td></tr>";
    return a;

}


QTextDocument *Export::createDocListe(QPrinter *printer)
{
    QTextDocument *doc = new QTextDocument(this);
    doc->setDocumentMargin(0);
    doc->setPageSize(QSizeF(printer->pageRect().size()));
    QString a = "<html><head><title>Einsatzplan</title></head><body>";
    a += "<style type='text/css'>";
    a += "body, tr, td, p { font-size: 80%;}";
    a += "table {border-width: 1px; border-style: solid; border-color: black; border-collapse: collapse; }";
    a += "table th, table td {border-width: 1px; border-collapse: collapse; padding: 0px;border-style: solid;border-color: black; }";
    a += "table tr { page-break-inside: avoid;}";
    a +="</style>";
    a += "<h3 page-break-before='always'>Übersicht über die Fahrtage</h3>";
    a += "<table><thead><tr> <th>Datum, Anlass</th> <th>Tf, Tb</th> <th><u>Zf</u>, Zub, Begl.o.b.A</th> <th>Service</th> <th>Dienstbeginn</th> <th>Sonstiges</th></tr></thead><tbody>";
    for(int i = 0; i < manager->getFahrtage()->length(); i++) {
        Fahrtag *f = manager->getFahrtage()->at(i);
        if (isAllowed(f)) {
            a += "<tr bgcolor='"+PlanerFahrtage::getFarbe(f->getArt()).name()+"'>";
            a += (f->getWichtig() ? "<td bgcolor='red'>": "<td>" );
            a += "<b>"+f->getDatum().toString("dddd d.M.yyyy")+"</b><br/>("+PlanerFahrtage::getArt(f->getArt())+")<br/>"+f->getAnlass()+"</td>";
            a += (f->getBenoetigeTf()?"<td><b>Lokführer werden benötigt!</b><br/>":"<td>")+listToString(f->getTf(), "|<br/>")+"</td>";
            if (f->getArt() != 3) {
                a += (f->getBenoetigeTf()?"<td><u><b>Zugführer wird benötigt!</b><br/>":"<td><u>")+listToString(f->getZf(), "|<br/>")+"</u><br/>";
                a += (f->getBenoetigeTf()?"<b>Zugbegleiter werden benötigt!</b><br/>":" ")+listToString(f->getZub(), "|<br/>")+"</td>";
                a += (f->getBenoetigeTf()?"<td><b>Service-Personal wird benötigt!</b><br/>":"<td>")+listToString(f->getService(), "<br/>")+"</td>";
            }
            a += "<td>Tf/Tb: "+f->getTimeTf().toString("hh:mm")+"<br/>";
            if (f->getArt() != 3) a += "Zf/Zub/Service: "+f->getTimeZ().toString("hh:mm");
            a += "</td>";
            a += "<td>"+f->getWagenreihung()+(f->getWagenreihung() != "" ? "<br/>": "");
            if (f->getArt() <= 3 && f->getManager()->getGesamtzahl() > 0) {
                a += QString::number(f->getManager()->getGesamtzahl());
                a += (f->getManager()->getGesamtzahl() == 1 ? " reservierter Sitzplatz": " reservierte Sitzplätze");
                a += "<br/>";
            }
            a += f->getBemerkungen()+"</td></tr>";
        }
    }
    a += "</tbody></table>";
    a += "</body></html>";
    doc->setHtml(a);

    return doc;

}

QString Export::listToString(QList<QString> *liste, QString deliminiter)
{
    QString a= "";
    for (int i = 0; i < liste->length(); i++) {
        a += liste->at(i);
        if (i+1 < liste->length()) {
            a += deliminiter;
        }
    }
    return a;
}

bool Export::isAllowed(Fahrtag *f)
{
    return ! map->key(f, new QListWidgetItem())->isHidden();
}

bool Export::isAllowedSingle(Fahrtag *f)
{
    return map->key(f, new QListWidgetItem())->isSelected() && ! map->key(f, new QListWidgetItem())->isHidden();
}




