#include "exportdialog.h"
#include "ui_exportdialog.h"
#include "export.h"
#include "eplexception.h"

#include <QMessageBox>
#include <QPrintPreviewDialog>

using namespace EplException;

ExportDialog::ExportDialog(Manager *m, FileSettings *settings, QWidget *parent) : QDialog(parent), ui(new Ui::ExportDialog)
{
    ui->setupUi(this);

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

    connect(ui->comboVon, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ExportDialog::changedFrom);
    connect(ui->comboBis, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ExportDialog::changedTill);

    connect(ui->pushPreview, &QPushButton::clicked, this, &ExportDialog::showPrintPreview);
    connect(ui->pushExportUpload, &QPushButton::clicked, this, &ExportDialog::exportUpload);
    connect(ui->pushExportPDF, &QPushButton::clicked, this, &ExportDialog::exportPDF);
    connect(ui->pushExportPrint, &QPushButton::clicked, this, &ExportDialog::exportPrint);

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

void ExportDialog::showPrintPreview()
{
    QList<AActivity*> liste = getAActivityForExport();

    QPrintPreviewDialog *prev;
    QPrinter *printer = new QPrinter();
    if (ui->buttonGroupExportArt->checkedId() == 0) {
        Export::preparePrinter(printer, QPageLayout::Orientation::Landscape);
        prev = new QPrintPreviewDialog(printer, parentWidget());
        connect(prev, &QPrintPreviewDialog::paintRequested, this, [=](QPrinter * print) { manager->exportActivitiesListAsHtml(liste, new Export(print)); } );
    } else {
        Export::preparePrinter(printer, QPageLayout::Orientation::Portrait);
        prev = new QPrintPreviewDialog(printer, parentWidget());
        connect(prev, &QPrintPreviewDialog::paintRequested, this, [=](QPrinter * print) { manager->exportActivitiesDetailAsHtml(liste, new Export(print)); } );
    }
    prev->exec(); // == QDialog::Accepted)
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

QList<AActivity *> ExportDialog::getAActivityForExport(bool ignoreSelectionStatus)
{
    ignoreSelectionStatus = ignoreSelectionStatus || ui->buttonGroupExportArt->checkedId() == 0;

    QList<AActivity*> liste = QList<AActivity*>();
    for(AActivity *a: manager->getActivities()) {
        if (actToList.value(a)->isSelected() || ignoreSelectionStatus) {
            if(! actToList.value(a)->isHidden()) {
                liste.append(a);
            }
        }
    }
    return liste;
}

void ExportDialog::exportUpload()
{
    QList<AActivity*> liste = getAActivityForExport(true);

    Export *printer;
    if (settings->getEnabled()) {
        printer = Export::getPrinterOnline(settings->getServer(), QPageLayout::Landscape);
        if (manager->exportActivitiesListAsHtml(liste, printer)) {
            QMessageBox::information(parentWidget(), tr("Erfolg"), tr("Datei wurde erfolgreich hochgeladen!"));
        } else {
            QMessageBox::warning(parentWidget(), tr("Fehler"), tr("Die Datei konnte nicht hochgeladen werden!"));
        }
    }
}

void ExportDialog::exportPDF()
{
    QList<AActivity*> liste = getAActivityForExport();

    Export *printer;
    if (ui->buttonGroupExportArt->checkedId() == 0) {
        printer = Export::getPrinterPDF(parentWidget(), QDate::currentDate().toString(tr("'Listenansicht'-yyyy-MM-dd")), QPageLayout::Orientation::Landscape);
        manager->exportActivitiesListAsHtml(liste, printer);
    } else {
        printer = Export::getPrinterPDF(parentWidget(), QDate::currentDate().toString(tr("'Einzelansicht'-yyyy-MM-dd")), QPageLayout::Orientation::Portrait);
        manager->exportActivitiesDetailAsHtml(liste, printer);
    }
}

void ExportDialog::exportPrint()
{
    QList<AActivity*> liste = getAActivityForExport();

    Export *printer;
    if (ui->buttonGroupExportArt->checkedId() == 0) {
        printer = Export::getPrinterPaper(parentWidget(), QPageLayout::Orientation::Landscape);
        manager->exportActivitiesListAsHtml(liste, printer);
    } else {
        printer = Export::getPrinterPaper(parentWidget(), QPageLayout::Orientation::Portrait);
        manager->exportActivitiesDetailAsHtml(liste, printer);
    }
}
