#include "exportdialog.h"
#include "ui_exportdialog.h"
#include "export.h"
#include "eplexception.h"

#include <QMessageBox>

using namespace EplException;

ExportDialog::ExportDialog(Manager *m, FileSettings *settings, QWidget *parent) : QDialog(parent), ui(new Ui::ExportDialog)
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

    connect(ui->dateVon, &QDateEdit::dateChanged, this, &ExportDialog::show);
    connect(ui->dateBis, &QDateEdit::dateChanged, this, &ExportDialog::show);
    connect(ui->comboFahrtag, &QComboBox::currentTextChanged, this, &ExportDialog::show);
    connect(ui->checkActivity, &QCheckBox::clicked, this, &ExportDialog::show);
    connect(ui->listAnzeige, &QListWidget::itemSelectionChanged, this, [=]() {
        ui->checkEinzel->setEnabled(! ui->listAnzeige->selectedItems().isEmpty()); });

    connect(ui->pushDrucken, &QPushButton::clicked, this, &ExportDialog::perfomExport);
    connect(ui->comboVon, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ExportDialog::changedFrom);
    connect(ui->comboBis, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ExportDialog::changedTill);

    hardReload();
}

ExportDialog::~ExportDialog()
{
    delete ui;
}

void ExportDialog::hardReload()
{
    actToList = QMap<AActivity*, QListWidgetItem*>();

    ui->listAnzeige->clear();
    for(AActivity *a: manager->getActivities()) {
        QString farbe = a->getFarbe();
        QListWidgetItem *item = new QListWidgetItem(a->getString());
        item->setBackground(QBrush(QColor(farbe)));
        item->setForeground(QBrush(QColor("black")));
        item->setToolTip(a->getAnlass().replace("<br/>","\n"));
        ui->listAnzeige->insertItem(ui->listAnzeige->count(), item);
        actToList.insert(a, item);
    }
    show();
}

void ExportDialog::perfomExport()
{
    QList<AActivity*> liste = QList<AActivity*>();
    for(AActivity *a: manager->getActivities()) {
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

    QPrinter *printer;
    if (ui->buttonGroupExportArt->checkedId() == 0) {
        switch (ui->buttonGroupExportFormat->checkedId()) {
        case 0:
            printer = Export::getPrinterPaper(parentWidget(), QPageLayout::Orientation::Landscape);
            break;
        case 1:
            printer = Export::getPrinterPDF(parentWidget(), tr("Listenansicht-%1").arg(QDate::currentDate().toString("YYYY-MM-dd")), QPageLayout::Orientation::Landscape);
            break;
        case 2:
            if (settings->getEnabled()) {
                try {
                    Export::uploadToServer(liste, settings->getServer());
                    QMessageBox::information(parentWidget(), tr("Erfolg"), tr("Datei wurde erfolgreich hochgeladen!"));
                }  catch (NetworkingException &e) {
                    QMessageBox::information(parentWidget(), tr("Fehler"), tr("Die Datei konnte nicht hochgeladen werden!"));
                }
            }
            return;
        default:
            return;
        }
        manager->printListenansicht(liste, printer);
    } else {
        switch (ui->buttonGroupExportFormat->checkedId()) {
        case 0:
            printer = Export::getPrinterPaper(parentWidget(), QPageLayout::Orientation::Portrait);
            break;
        case 1:
            printer = Export::getPrinterPDF(parentWidget(), tr("Einzelansicht-%1").arg(QDate::currentDate().toString("YYYY-MM-dd")), QPageLayout::Orientation::Portrait);
            break;
        default:
            return;
        }
        manager->printEinzelansichten(liste, printer);
    }
}

void ExportDialog::changedFrom(int index)
{
    switch (index) {
    case 0: // Ab datum
        ui->dateVon->setEnabled(true);
        break;
    case 1: // Ab jetzt
    case 2: // Ab beginn des Jahres
    case 3: // Ab egal
        ui->dateVon->setEnabled(false);
        break;
    default:
        ui->dateVon->setEnabled(true);
    }
    show();
}

void ExportDialog::changedTill(int index)
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

void ExportDialog::show()
{
    ui->checkListe->setEnabled(false);
    for(AActivity *a: manager->getActivities()) {
        if(testShow(a)) {
            actToList.value(a)->setHidden(false);
            ui->checkListe->setEnabled(true);
        } else {
            actToList.value(a)->setHidden(true);
        }
    }
}

bool ExportDialog::testShow(AActivity *a)
{
    QDate abD = ui->dateVon->date();
    Auswahl::AnfangBedingung ab = Auswahl::AbAlle;
    // Prüfen bei den Einträgen, ob das Datum stimmt
    switch (ui->comboVon->currentIndex()) {
    case 0: // Ab datum
        ab = Auswahl::AbDatum;
        break;
    case 1: // Ab jetzt
        ab = Auswahl::AbJetzt;
        break;
    case 2: // Ab beginn des Jahres
        ab = Auswahl::AbAnfangDesJahres;
        break;
    case 3: // Ab egal
        ab = Auswahl::AbAlle;
        break;
    default:
        break;
    }

    QDate bisD = ui->dateBis->date();
    Auswahl::EndeBedingung bis = Auswahl::BisEndeDesJahres;
    switch (ui->comboBis->currentIndex()) {
    case 0: // Bis datum
        bis = Auswahl::BisDatum;
        break;
    case 1: // Bis heute
        bis = Auswahl::BisHeute;
        break;
    case 2: // Bis Ende des Jahres
        bis = Auswahl::BisEndeDesJahres;
        break;
    case 3: // Bis egal
        bis = Auswahl::BisAlle;
        break;
    default:
        break;
    }
    if (! a->check(Auswahl(ab, abD, bis, bisD)))
        return false;

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
