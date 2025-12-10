#include "exportdialog.h"
#include "activitymodel.h"
#include "ui_exportdialog.h"
#include "export.h"

#include <QMessageBox>
#include <QPrintPreviewDialog>

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
    ActivityModel *model = new ActivityModel(manager);
    filterModel = new ActivityFilterModel();
    filterModel->setSource(model);
    ui->listAnzeige->setModel(filterModel);
    ui->listAnzeige->setModelColumn(3);
    ui->listAnzeige->show();

    connect(ui->dateVon, &QDateEdit::dateChanged, this, &ExportDialog::changedFrom);
    connect(ui->comboVon, &QComboBox::currentIndexChanged, this, &ExportDialog::changedFrom);

    connect(ui->dateBis, &QDateEdit::dateChanged, this, &ExportDialog::changedTill);
    connect(ui->comboBis, &QComboBox::currentIndexChanged, this, &ExportDialog::changedTill);

    connect(ui->comboFahrtag, &QComboBox::currentTextChanged, this, &ExportDialog::changedType);
    connect(ui->checkActivity, &QCheckBox::clicked, this, &ExportDialog::changedType);

    connect(ui->pushPreview, &QPushButton::clicked, this, &ExportDialog::showPrintPreview);
    connect(ui->pushExportUpload, &QPushButton::clicked, this, &ExportDialog::exportUpload);
    connect(ui->pushExportPDF, &QPushButton::clicked, this, &ExportDialog::exportPDF);
    connect(ui->pushExportPrint, &QPushButton::clicked, this, &ExportDialog::exportPrint);

    changedFrom();
    changedTill();
    changedType();
}

ExportDialog::~ExportDialog()
{
    delete ui;
}

void ExportDialog::changedFrom()
{
    QDate ref = QDate::currentDate();
    QDateTime startTime;
    switch (ui->comboVon->currentIndex()) {
    case 0: // From given date on
        startTime = ui->dateVon->date().startOfDay();
        ui->dateVon->setEnabled(true);
        break;
    case 1: // From now on
        startTime = QDateTime::currentDateTime();
        ui->dateVon->setEnabled(false);
        break;
    case 2: // From start of this year
        startTime = QDate(ref.year(), 1, 1).startOfDay();
        ui->dateVon->setEnabled(false);
        break;
    case 3: // All past events
        startTime = QDate(1900, 1, 1).startOfDay();
        ui->dateVon->setEnabled(false);
        break;
    default:
        // From today on
        startTime = ref.startOfDay();
        ui->dateVon->setEnabled(true);
        break;
    }
    filterModel->setDateStart(startTime);
}

void ExportDialog::changedTill()
{
    QDate ref = QDate::currentDate();
    QDateTime endTime;
    switch (ui->comboBis->currentIndex()) {
    case 0: // Until given date
        endTime = ui->dateBis->date().endOfDay();
        ui->dateBis->setEnabled(true);
        break;
    case 1: // Until today
        endTime = ref.endOfDay();
        ui->dateBis->setEnabled(false);
        break;
    case 2: // Until the end of this year
        endTime = QDate(ref.year(), 12, 31).endOfDay();
        ui->dateBis->setEnabled(false);
        break;
    case 3: // All future events
        endTime = QDate(9999, 12, 31).endOfDay();
        ui->dateBis->setEnabled(false);
        break;
    default:
        endTime = ref.endOfDay();
        ui->dateBis->setEnabled(true);
        break;
    }

    filterModel->setDateEnd(endTime);
}

void ExportDialog::changedType()
{
    int i = ui->comboFahrtag->currentIndex();
    QSet<Art> filter = {};
    if (i == 9) {
    } else if (i == 8) {
        filter << Museumszug
               << Sonderzug
               << Gesellschaftssonderzug
               << Nikolauszug
               << ELFundMuseumszug
               << Schnupperkurs
               << Bahnhofsfest
               << SonstigerFahrtag;
    } else {
        filter.insert((Art)i);
    }
    if (ui->checkActivity->isChecked()) {
        filter << Arbeitseinsatz;
    }
    filterModel->setAcceptedTypes(filter);
}

void ExportDialog::showPrintPreview()
{
    QList<AActivity*> liste = getAActivityForExport();

    QPrintPreviewDialog *prev;
    QPrinter *printer = new QPrinter();
    if (ui->buttonGroupExportArt->checkedId() == 0) {
        Export::prepareLayout(printer, QPageLayout::Orientation::Landscape);
        prev = new QPrintPreviewDialog(printer, parentWidget());
        connect(prev, &QPrintPreviewDialog::paintRequested, this, [=](QPrinter * print) { manager->exportActivitiesListAsHtml(liste, new ExportHtmlPaper(print)); } );
    } else {
        Export::prepareLayout(printer, QPageLayout::Orientation::Portrait);
        prev = new QPrintPreviewDialog(printer, parentWidget());
        connect(prev, &QPrintPreviewDialog::paintRequested, this, [=](QPrinter * print) { manager->exportActivitiesDetailAsHtml(liste, new ExportHtmlPaper(print)); } );
    }
    prev->exec(); // == QDialog::Accepted)
}

QList<AActivity *> ExportDialog::getAActivityForExport(bool ignoreSelectionStatus)
{
    QModelIndexList indexList;
    if (ignoreSelectionStatus || ui->buttonGroupExportArt->checkedId() == 0) {
        for (int i = 0; i < filterModel->rowCount(); ++i) {
            indexList.append(filterModel->index(i, 0));
        }
    } else {
        indexList = ui->listAnzeige->selectionModel()->selection().indexes();
    }

    QList<AActivity*> liste = QList<AActivity*>();
    for(QModelIndex index: indexList) {
        QModelIndex indexSource = filterModel->mapToSource(index);
        liste.append(filterModel->sourceModel()->getData(indexSource));
    }
    AActivity::sort(&liste);
    return liste;
}

void ExportDialog::exportUpload()
{
    QList<AActivity*> liste = getAActivityForExport(true);

    ExportHtml *printer;
    if (settings->getEnabled()) {
        printer = new ExportHtmlUpload(settings->getServer(), QPageLayout::Landscape);
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

    ExportHtml *printer;
    if (ui->buttonGroupExportArt->checkedId() == 0) {
        printer = ExportHtmlPdf::generate(parentWidget(), QDate::currentDate().toString(tr("'Listenansicht'-yyyy-MM-dd")), QPageLayout::Orientation::Landscape);
        manager->exportActivitiesListAsHtml(liste, printer);
    } else {
        printer = ExportHtmlPdf::generate(parentWidget(), QDate::currentDate().toString(tr("'Einzelansicht'-yyyy-MM-dd")), QPageLayout::Orientation::Portrait);
        manager->exportActivitiesDetailAsHtml(liste, printer);
    }
}

void ExportDialog::exportPrint()
{
    QList<AActivity*> liste = getAActivityForExport();

    ExportHtml *printer;
    if (ui->buttonGroupExportArt->checkedId() == 0) {
        printer = ExportHtmlPaper::generate(parentWidget(), QPageLayout::Orientation::Landscape);
        manager->exportActivitiesListAsHtml(liste, printer);
    } else {
        printer = ExportHtmlPaper::generate(parentWidget(), QPageLayout::Orientation::Portrait);
        manager->exportActivitiesDetailAsHtml(liste, printer);
    }
}
