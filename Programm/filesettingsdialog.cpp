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
    case 0: auswahl.startdate = "tdy"; break;
    case 1: auswahl.startdate = "bgn"; break;
    case 2: auswahl.startdate = "all"; break;
    default: auswahl.startdate = "tdy"; break;
    }
    switch (ui->comboTo->currentIndex()) {
    case 0: auswahl.enddate = "p1w"; break;
    case 1: auswahl.enddate = "p1m"; break;
    case 2: auswahl.enddate = "eoy"; break;
    default: auswahl.enddate = "all"; break;
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
    QString start = a.startdate;
    if (start == "tdy") {
        ui->comboFrom->setCurrentIndex(0);
    } else if (start == "bgn") {
        ui->comboFrom->setCurrentIndex(1);
    } else if (start == "all") {
        ui->comboFrom->setCurrentIndex(2);
    }
    QString end = a.enddate;
    if (end == "p1w") {
        ui->comboTo->setCurrentIndex(0);
    } else if (end == "p1m") {
        ui->comboTo->setCurrentIndex(1);
    } else if (end == "eoy") {
        ui->comboTo->setCurrentIndex(2);
    } else if (end == "all") {
        ui->comboTo->setCurrentIndex(3);
    }
    ui->checkActivity->setChecked(a.activities);
    on_checkEnable_clicked(mngr->getEnabled());
}
