#include "filesettings.h"
#include "export.h"
#include "ui_filesettings.h"

#include <QMessageBox>

FileSettings::FileSettings(QWidget *parent, ManagerFileSettings *manager) : QDialog(parent), ui(new Ui::FileSettings)
{
    mngr = manager;
    ui->setupUi(this);
    loadSettings();
}

FileSettings::~FileSettings()
{
    delete ui;
}

void FileSettings::getSettings(ManagerFileSettings *mgr)
{
    mgr->setEnabled(ui->checkEnable->isChecked());
    mgr->setAutom(ui->checkAuto->isChecked());
    mgr->setServer(ui->lineServer->text());
    mgr->setPath(ui->linePath->text());
    mgr->setId(ui->lineID->text());

    switch (ui->comboFrom->currentIndex()) {
    case 0: mgr->setStartdate("tdy"); break;
    case 1: mgr->setStartdate("bgn"); break;
    case 2: mgr->setStartdate("all"); break;
    default: mgr->setStartdate("tdy"); break;
    }
    switch (ui->comboTo->currentIndex()) {
    case 0: mgr->setEnddate("eow"); break;
    case 1: mgr->setEnddate("eom"); break;
    case 2: mgr->setEnddate("eoy"); break;
    default: mgr->setEnddate("all"); break;
    }
    mgr->setActivities(ui->checkActivity->isChecked());
}

void FileSettings::on_checkEnable_clicked(bool checked)
{
    ui->checkAuto->setEnabled(checked);
    ui->lineServer->setEnabled(checked);
    ui->linePath->setEnabled(checked);
    ui->lineID->setEnabled(checked);
    ui->pushCheck->setEnabled(checked);
    ui->labelStatus->setEnabled(checked);
    ui->comboFrom->setEnabled(checked);
    ui->comboTo->setEnabled(checked);
    ui->checkActivity->setEnabled(checked);
}

void FileSettings::on_buttonBox_clicked(QAbstractButton *button)
{
    if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole) {
        loadSettings();
    }
}

void FileSettings::on_pushCheck_clicked()
{
    if(Export::testServerConnection(ui->lineServer->text(), ui->linePath->text(), ui->lineID->text())) {
        ui->labelStatus->setText(tr("Verbindung erfolgreich getestet!"));
    } else {
        ui->labelStatus->setText(tr("Verbindung zum Server fehlgeschlagen!"));
    }
}

void FileSettings::loadSettings()
{
    ui->checkEnable->setChecked(mngr->getEnabled());
    ui->checkAuto->setChecked(mngr->getAutom());
    ui->lineServer->setText(mngr->getServer());
    ui->linePath->setText(mngr->getPath());
    ui->lineID->setText(mngr->getId());
    QString start = mngr->getStartdate();
    if (start == "tdy") {
        ui->comboFrom->setCurrentIndex(0);
    } else if (start == "bgn") {
        ui->comboFrom->setCurrentIndex(1);
    } else if (start == "all") {
        ui->comboFrom->setCurrentIndex(2);
    }
    QString end = mngr->getEnddate();
    if (end == "eow") {
        ui->comboTo->setCurrentIndex(0);
    } else if (end == "eom") {
        ui->comboTo->setCurrentIndex(1);
    } else if (end == "eoy") {
        ui->comboTo->setCurrentIndex(2);
    } else if (end == "all") {
        ui->comboTo->setCurrentIndex(3);
    }
    ui->checkActivity->setChecked(mngr->getActivities());
    on_checkEnable_clicked(mngr->getEnabled());
}
