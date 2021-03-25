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

void FileSettingsDialog::saveSettings()
{
    mngr->setEnabled(ui->checkEnable->isChecked());
    mngr->setAutom(ui->checkAuto->isChecked());
    mngr->setServer(Networking::Server{
                       ui->lineServer->text(),
                       ui->linePath->text(),
                       ui->lineID->text()}
                       );

    Auswahl::AnfangBedingung ab = Auswahl::AbJetzt;
    switch (ui->comboFrom->currentIndex()) {
    case 0: ab = Auswahl::AnfangBedingung::AbJetzt; break;
    case 1: ab = Auswahl::AnfangBedingung::AbAnfangDesJahres; break;
    case 2: ab = Auswahl::AnfangBedingung::AbAlle; break;
    default: ab = Auswahl::AnfangBedingung::AbJetzt; break;
    }
    Auswahl::EndeBedingung bis = Auswahl::BisAlle;
    switch (ui->comboTo->currentIndex()) {
    case 0: bis = Auswahl::EndeBedingung::BisEndeNaechsterWoche; break;
    case 1: bis = Auswahl::EndeBedingung::BisEndeNaechsterMonat; break;
    case 2: bis = Auswahl::EndeBedingung::BisEndeDesJahres; break;
    default: bis = Auswahl::EndeBedingung::BisAlle; break;
    }
    mngr->setAuswahl(Auswahl(ab, bis, ui->checkActivity->isChecked()));
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
    } else if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::AcceptRole) {
        saveSettings();
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
    Auswahl a = mngr->getAuswahl();
    switch (a.getStartdate()) {
    case Auswahl::AnfangBedingung::AbJetzt:
        ui->comboFrom->setCurrentIndex(0);
        break;
    case Auswahl::AnfangBedingung::AbAnfangDesJahres:
        ui->comboFrom->setCurrentIndex(1);
        break;
    case Auswahl::AnfangBedingung::AbAlle:
        ui->comboFrom->setCurrentIndex(2);
        break;
    default:
        ui->comboFrom->setCurrentIndex(2);
    }
    switch (a.getEnddate()) {
    case Auswahl::EndeBedingung::BisEndeNaechsterWoche:
        ui->comboTo->setCurrentIndex(0);
        break;
    case Auswahl::EndeBedingung::BisEndeNaechsterMonat:
        ui->comboTo->setCurrentIndex(1);
        break;
    case Auswahl::EndeBedingung::BisEndeDesJahres:
        ui->comboTo->setCurrentIndex(2);
        break;
    case Auswahl::EndeBedingung::BisAlle:
        ui->comboTo->setCurrentIndex(3);
        break;
    default:
        ui->comboTo->setCurrentIndex(3);
    }
    ui->checkActivity->setChecked(a.getActivities());
    on_checkEnable_clicked(mngr->getEnabled());

    ui->linePwdAlt->setEnabled(mngr->getPasswort() != "");
}

void FileSettingsDialog::on_pushPwdChange_clicked()
{
    QString alt = ui->linePwdAlt->text();
    QString neu1 = ui->linePwdNeu1->text();
    QString neu2 = ui->linePwdNeu2->text();
    ui->linePwdAlt->clear();
    ui->linePwdNeu1->clear();
    ui->linePwdNeu2->clear();

    if (neu1 != neu2) {
        QMessageBox::information(this, tr("Fehler"), tr("Die beiden neuen Passwörter stimmen nicht überein!"));
        return;
    }
    if (! mngr->setPasswort(neu1, alt)) {
        QMessageBox::information(this, tr("Fehler"), tr("Das Passwort konnte nciht geöndert werden, überprüfen Sie Ihre Eingabe."));
        return;
    }
    QMessageBox::information(this, tr("Geändert"), tr("Das Passwort wurde erfolgreich geändert."));

    ui->linePwdAlt->setEnabled(mngr->getPasswort() != "");
}
