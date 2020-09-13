#include "exportgesamt.h"
#include "ui_exportgesamt.h"
#include "export.h"

#include <QMessageBox>

ExportGesamt::ExportGesamt(Manager *m, ManagerFileSettings *settings, QWidget *parent) : QDialog(parent), ui(new Ui::ExportGesamt)
{
    ui->setupUi(this);
    ui->buttonGroupExportFormat->setId(ui->checkAusdruck, 0);
    ui->buttonGroupExportFormat->setId(ui->checkPDF, 1);
    ui->buttonGroupExportFormat->setId(ui->checkUpload, 2);
    ui->checkAusdruck->click();

    ui->buttonGroupExportArt->setId(ui->checkListe, 0);
    ui->buttonGroupExportArt->setId(ui->checkEinzel, 1);
    ui->checkListe->click();

    p = parent;
    this->settings = settings;
    ui->dateVon->setDate(QDate::currentDate());
    ui->dateBis->setDate(QDate::currentDate().addDays(60)); // Anzeige für zwei Monate in der Zukunft
    manager = m;

    connect(ui->dateVon, &QDateEdit::dateChanged, this, &ExportGesamt::show);
    connect(ui->dateBis, &QDateEdit::dateChanged, this, &ExportGesamt::show);
    connect(ui->comboFahrtag, &QComboBox::currentTextChanged, this, &ExportGesamt::show);
    connect(ui->checkActivity, &QCheckBox::clicked, this, &ExportGesamt::show);
    connect(ui->listAnzeige, &QListWidget::itemSelectionChanged, this, [=]() {
        ui->checkEinzel->setEnabled(! ui->listAnzeige->selectedItems().isEmpty()); });

    hardReload();
}

ExportGesamt::~ExportGesamt()
{
    delete ui;
}

void ExportGesamt::hardReload()
{
    actToList = QMap<AActivity*, QListWidgetItem*>();

    ui->listAnzeige->clear();
    foreach(AActivity *a, manager->getActivities()) {
        QString farbe = getFarbe(a);
        QListWidgetItem *item = new QListWidgetItem(a->getListString());
        item->setBackground(QBrush(QColor(farbe)));
        item->setToolTip(a->getAnlass().replace("<br/>","\n"));
        ui->listAnzeige->insertItem(ui->listAnzeige->count(), item);
        actToList.insert(a, item);
    }
    show();
}

void ExportGesamt::on_pushDrucken_clicked()
{
    QList<AActivity*> liste = QList<AActivity*>();
    foreach(AActivity *a, manager->getActivities()) {
        switch (ui->buttonGroupExportArt->checkedId()) {
        case 1:
            if(!(actToList.value(a)->isSelected())) {
                continue;
            }
        case 0:
            if(! actToList.value(a)->isHidden()) {
                liste.append(a);
            }
            break;
        default:
            continue;
        }
    }
    if (liste.isEmpty()) return;


    QPrinter *printer;
    if (ui->buttonGroupExportArt->checkedId() == 0) {
        switch (ui->buttonGroupExportFormat->checkedId()) {
        case 0:
            printer = Export::getPrinterPaper(parentWidget(), QPrinter::Orientation::Landscape);
            break;
        case 1:
            printer = Export::getPrinterPDF(parentWidget(), "Listenansicht.pdf", QPrinter::Orientation::Landscape);
            break;
        case 2:
            if (Export::uploadToServer(settings, liste)) {
                QMessageBox::information(parentWidget(), tr("Erfolg"), tr("Datei wurde erfolgreich hochgeladen!"));
            } else {
                QMessageBox::information(parentWidget(), tr("Fehler"), tr("Die Datei konnte nicht hochgeladen werden!"));
            }
            return;
        default:
            return;
        }
        Export::printAktivitaetenListe(liste, printer);
    } else {
        switch (ui->buttonGroupExportFormat->checkedId()) {
        case 0:
            printer = Export::getPrinterPaper(parentWidget(), QPrinter::Orientation::Portrait);
            break;
        case 1:
            printer = Export::getPrinterPDF(parentWidget(), "Einzelansicht.pdf", QPrinter::Orientation::Portrait);
            break;
        default:
            return;
        }
        Export::printAktivitaetenEinzel(liste, printer);
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

void ExportGesamt::show()
{
    ui->checkListe->setEnabled(false);
    foreach(AActivity *a, manager->getActivities()) {
        if(testShow(a)) {
            actToList.value(a)->setHidden(false);
            ui->checkListe->setEnabled(true);
        } else {
            actToList.value(a)->setHidden(true);
        }
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
        if (a->liegtInVergangenheit())
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
    if ((a->getArt() != Art::Arbeitseinsatz)) {
        int i = ui->comboFahrtag->currentIndex();
        if (i == 9) return false;
        if (i != 8) {
            if (i != a->getArt()) {
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
