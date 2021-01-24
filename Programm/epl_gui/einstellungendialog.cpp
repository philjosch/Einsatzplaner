#include "einstellungendialog.h"
#include "ui_einstellungendialog.h"
#include "coreapplication.h"
#include "einstellungen.h"

#include <QDesktopServices>
#include <QMessageBox>

EinstellungenDialog::EinstellungenDialog(QWidget *parent) : QDialog(parent), ui(new Ui::EinstellungenDialog)
{
    ui->setupUi(this);
    ui->buttonGroupSortierung->setId(ui->radioVorNach, 1);
    ui->buttonGroupSortierung->setId(ui->radioNachVor, 0);
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(closeDialogOk()));
    connect(ui->pushDownload, &QPushButton::clicked, this, [=]() { CoreApplication::oeffneDownloadSeite(); });
    ui->checkSearchAtStart->setChecked(Einstellungen::getAutoSearchUpdate());
    ui->checkAutoUpload->setChecked(Einstellungen::getUseAutoUpload());
    int index = Einstellungen::getAutoSave();
    if (index == 5) {
        ui->comboAutoSave->setCurrentIndex(0);
    } else if (index == 10) {
        ui->comboAutoSave->setCurrentIndex(1);
    } else if (index == 15) {
        ui->comboAutoSave->setCurrentIndex(2);
    } else if (index == 30) {
        ui->comboAutoSave->setCurrentIndex(3);
    }
    ui->buttonGroupSortierung->button(Einstellungen::getReihenfolgeVorNach() ? 1 : 0)->click();
    setWindowFilePath("");
}

EinstellungenDialog::~EinstellungenDialog()
{
    delete ui;
}

void EinstellungenDialog::on_pushSearch_clicked()
{
    if (Version::isUpdateVerfuegbar()) {
        online = Version::ladeNeusteVersion();
        ui->labelVersion->setText(
                    tr("Sie verwenden Version %1. Es ist Version %2 verfügbar.")
                    .arg(QCoreApplication::applicationVersion(), online.toString()));
        ui->pushDownload->setEnabled(true);
        ui->pushNotes->setEnabled(true);
    } else {
        ui->labelVersion->setText(
                    tr("Sie verwenden bereits die neuste Version %1.")
                    .arg(QCoreApplication::applicationVersion()));
        ui->pushDownload->setEnabled(false);
        ui->pushNotes->setEnabled(false);
    }
}

void EinstellungenDialog::closeDialogOk()
{
    saveSettings();
    this->accept();
}

void EinstellungenDialog::saveSettings()
{
    Einstellungen::setAutoSearchUpdate(ui->checkSearchAtStart->isChecked());
    int value = 0;
    int index = ui->comboAutoSave->currentIndex();
    if (index == 0) {
        value = 5;
    } else if (index == 1) {
        value = 10;
    } else if (index == 2) {
        value = 15;
    } else if (index == 3) {
        value = 30;
    }
    Einstellungen::setAutoSave(value);
    Einstellungen::setUseAutoUpload(ui->checkAutoUpload->isChecked());
    Einstellungen::setReihenfolgeVorNach(ui->buttonGroupSortierung->checkedId() == 1);
}

void EinstellungenDialog::on_pushNotes_clicked()
{
    QMessageBox::information(nullptr,
                             tr("Über Version %1").arg(online.toString()),
                             Version::loadNotes(online),
                             QMessageBox::Ok, QMessageBox::Ok);
}
