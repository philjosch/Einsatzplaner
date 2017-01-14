#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include "coreapplication.h"

#include <QDesktopServices>
#include <QSettings>
#include <QUrl>
#include <QMessageBox>

PreferencesDialog::PreferencesDialog(QWidget *parent) : QDialog(parent), ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(closeDialogOk()));
    QSettings settings;
    ui->checkSearchAtStart->setChecked(settings.value("general/autosearchupdate", true).toBool());
    setWindowFilePath("");
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::on_pushSearch_clicked()
{
    CoreApplication::Version *aktuell = CoreApplication::getAktuelleVersion();
    online = CoreApplication::loadVersion();
    bool old = online>aktuell;
    QString s;
    if (old) {
        s = tr("Sie verwenden Version %1. Es ist Version %2 verfügbar.").arg(aktuell->toString(), online.toString());
    } else {
        s =  tr("Sie verwenden bereits die neuste Version: %1.").arg(aktuell->toString());
    }
    ui->labelVersion->setText(s);
    ui->pushDownload->setEnabled(old);
    ui->pushNotes->setEnabled(old);
}

void PreferencesDialog::on_pushDownload_clicked()
{
    QDesktopServices::openUrl(CoreApplication::getUrlDownload());
}

void PreferencesDialog::closeDialogOk()
{
    saveSettings();
    this->accept();
}

void PreferencesDialog::saveSettings()
{
    QSettings settings;
    settings.setValue("general/autosearchupdate", ui->checkSearchAtStart->isChecked());
}

void PreferencesDialog::on_pushNotes_clicked()
{
    QMessageBox::information(nullptr, tr("Über Version ")+online.toString(), CoreApplication::loadNotes(online), QMessageBox::Ok, QMessageBox::Ok);
}
