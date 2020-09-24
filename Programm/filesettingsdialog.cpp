#include "filesettingsdialog.h"
#include "export.h"
#include "ui_filesettingsdialog.h"
#include "einstellungen.h"
#include "networking.h"

#include <QMessageBox>

FileSettingsDialog::FileSettingsDialog(QWidget *parent, FileSettings *manager) : QDialog(parent), ui(new Ui::FileSettingsDialog)
{
    mngr = manager;
    ui->setupUi(this);
    loadSettings();
}

FileSettingsDialog::~FileSettingsDialog()
{
    delete ui;
}

void FileSettingsDialog::getSettings(FileSettings *mgr)
{
    mgr->setEnabled(ui->checkEnable->isChecked());
    mgr->setAutom(ui->checkAuto->isChecked());
    mgr->setServer(Networking::Server{
                       ui->lineServer->text(),
                       ui->linePath->text(),
                       ui->lineID->text()}
                       );

    AActivity::Auswahl auswahl;
    switch (ui->comboFrom->currentIndex()) {
    case 0: auswahl.startdate = AnfangBedingung::AbJetzt; break;
    case 1: auswahl.startdate = AnfangBedingung::AbAnfangDesJahres; break;
    case 2: auswahl.startdate = AnfangBedingung::AbAlle; break;
    default: auswahl.startdate = AnfangBedingung::AbJetzt; break;
    }
    switch (ui->comboTo->currentIndex()) {
    case 0: auswahl.enddate = EndeBedingung::BisEndeNaechsterWoche; break;
    case 1: auswahl.enddate = EndeBedingung::BisEndeNaechsterMonat; break;
    case 2: auswahl.enddate = EndeBedingung::BisEndeDesJahres; break;
    default: auswahl.enddate = EndeBedingung::BisAlle; break;
    }
    auswahl.activities = ui->checkActivity->isChecked();
}

void FileSettingsDialog::on_checkEnable_clicked(bool checked)
{
    if (Einstellungen::getUseAutoUpload() && checked)
        ui->checkAuto->setEnabled(true);
    else
        ui->checkAuto->setEnabled(false);

    ui->lineServer->setEnabled(checked);
    ui->linePath->setEnabled(checked);
    ui->lineID->setEnabled(checked);
    ui->pushCheck->setEnabled(checked);
    ui->labelStatus->setEnabled(checked);
    ui->comboFrom->setEnabled(checked);
    ui->comboTo->setEnabled(checked);
    ui->checkActivity->setEnabled(checked);
}

void FileSettingsDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole) {
        loadSettings();
    }
}

void FileSettingsDialog::on_pushCheck_clicked()
{
    Networking::Server m2 = Networking::Server();
    m2.server = ui->lineServer->text();
    m2.path = ui->linePath->text();
    m2.id = ui->lineID->text();
    if (Networking::testServerVerbindung(m2)) {
        ui->labelStatus->setText(tr("Verbindung erfolgreich getestet!"));
    } else {
        ui->labelStatus->setText(tr("Verbindung zum Server fehlgeschlagen!"));
    }
}

void FileSettingsDialog::loadSettings()
{
    ui->checkEnable->setChecked(mngr->getEnabled());
    ui->checkAuto->setChecked(mngr->getAutom());
    Networking::Server s = mngr->getServer();
    ui->lineServer->setText(s.server);
    ui->linePath->setText(s.path);
    ui->lineID->setText(s.id);
    AActivity::Auswahl a = mngr->getAuswahl();
    switch (a.startdate) {
    case AnfangBedingung::AbJetzt:
        ui->comboFrom->setCurrentIndex(0);
        break;
    case AnfangBedingung::AbAnfangDesJahres:
        ui->comboFrom->setCurrentIndex(1);
        break;
    case AnfangBedingung::AbAlle:
        ui->comboFrom->setCurrentIndex(2);
        break;
    default:
        ui->comboFrom->setCurrentIndex(2);
    }
    switch (a.enddate) {
    case EndeBedingung::BisEndeNaechsterWoche:
        ui->comboTo->setCurrentIndex(0);
        break;
    case EndeBedingung::BisEndeNaechsterMonat:
        ui->comboTo->setCurrentIndex(1);
        break;
    case EndeBedingung::BisEndeDesJahres:
        ui->comboTo->setCurrentIndex(2);
        break;
    case EndeBedingung::BisAlle:
        ui->comboTo->setCurrentIndex(3);
        break;
    default:
        ui->comboTo->setCurrentIndex(3);
    }
    ui->checkActivity->setChecked(a.activities);
    on_checkEnable_clicked(mngr->getEnabled());
}
