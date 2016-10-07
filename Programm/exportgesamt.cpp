#include "exportgesamt.h"
#include "ui_exportgesamt.h"
#include "mainwindow.h"
#include "export.h"

#include <QPrinter>
#include <QPrintDialog>

ExportGesamt::ExportGesamt(Manager *m, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportGesamt)
{
    ui->setupUi(this);
    p = parent;
    ui->dateVon->setDate(QDate::currentDate());
    ui->dateBis->setDate(QDate::currentDate().addDays(60)); // Anzeige für zwei Monate in der Zukunft
    manager = m;

    // Daten in die Liste laden
    QListIterator<AActivity*> i = manager->getActivities();
    liste = QList<AActivity*>();
    actToList = QMap<AActivity*, QListWidgetItem*>();
    listToAct = QMap<QListWidgetItem*, AActivity*>();

    while(i.hasNext()) {
        AActivity *a = i.next();
        QString farbe = MainWindow::getFarbe(a);
        QListWidgetItem *item = new QListWidgetItem(a->getListString());
        item->setBackgroundColor(QColor(farbe));
        item->setToolTip(a->getAnlass());
        ui->listAnzeige->insertItem(ui->listAnzeige->count(), item);
        liste.append(a);
        actToList.insert(a, item);
        listToAct.insert(item, a);
    }

    //Anzeigen der Daten
    show();
}

ExportGesamt::~ExportGesamt()
{
    delete ui;
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
    QPrinter *paper = 0;
    QPrinter *pdfListe = 0;
    QPrinter *pdfEinzel = 0;
    if ((ui->checkAusdruck->isChecked()) && (ui->checkEinzel->isChecked() || ui->checkListe->isChecked()))
        paper = Export::getPrinterPaper(p);
    if (ui->checkPDF->isChecked()) {
        if (ui->checkEinzel->isChecked())
            pdfEinzel = Export::getPrinterPDF(p, "Einzelansicht.pdf");
        if (ui->checkListe->isChecked())
            pdfListe = Export::getPrinterPDF(p, "Listenansicht.pdf");
    }

    //Drucke die Daten mit dem Export modul
    if (ui->checkEinzel->isChecked() && listeEinzel->length() > 0)
        Export::printSingle(listeEinzel, pdfEinzel, paper);
    if (ui->checkListe->isChecked() && listeListe->length() > 0)
        Export::printList(listeListe, pdfListe, paper);
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
        if (a->getDatum().year() != QDate::currentDate().year())
            return false;
        break;
    case 3: // Ab egal
        break;
    default:
        break;
    }

    switch (ui->comboBis->currentIndex()) {
    case 0: // Bis datum
        if (a->getDatum() > ui->dateVon->date())
            return false;
        break;
    case 1: // Bis heute
        if (a->getDatum() > QDate::currentDate())
            return false;
        break;
    case 2: // Bis Ende des Jahres
        if (a->getDatum().year() != QDate::currentDate().year())
            return false;
        break;
    case 3: // Bis egal
        break;
    default:
        break;
    }

    bool ok = true;


    // Prüfen, ob die Art stimmt (Fahrtag, Arbeitseinsatz)
    if (Fahrtag *f = dynamic_cast<Fahrtag*>(a)) {
        int i = ui->comboFahrtag->currentIndex();
        if (i == 9) return false;
        if (i != 8) {
            if (i != (int)f->getArt()) {
                return false;
            }
        }
        return true;
        // es ist ein fahrtag
    } else {
        return ui->checkActivity->isChecked();
        // es ist kein fahrtag
    }


    return ok;
}
