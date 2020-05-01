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
    connect(ui->pushDownload, &QPushButton::clicked, this, [=]() { QDesktopServices::openUrl(CoreApplication::URL_DOWNLOAD); });
    QSettings settings;
    ui->checkSearchAtStart->setChecked(settings.value("general/autosearchupdate", true).toBool());
    ui->checkAutoUpload->setChecked(settings.value("online/useautoupload", true).toBool());
    int index = settings.value("io/autoSave", 4).toInt();
    if (index == 5) {
        ui->comboAutoSave->setCurrentIndex(0);
    } else if (index == 10) {
        ui->comboAutoSave->setCurrentIndex(1);
    } else if (index == 15) {
        ui->comboAutoSave->setCurrentIndex(2);
    } else if (index == 30) {
        ui->comboAutoSave->setCurrentIndex(3);
    }
    setWindowFilePath("");
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::on_pushSearch_clicked()
{
    Version aktuell = CoreApplication::VERSION;
    online = CoreApplication::loadVersion();
    bool old = online>aktuell;
    QString s;
    if (old) {
        s = tr("Sie verwenden Version %1. Es ist Version %2 verfügbar.").arg(aktuell.toString(), online.toString());
    } else {
        s = tr("Sie verwenden bereits die neuste Version %1.").arg(aktuell.toString());
    }
    ui->labelVersion->setText(s);
    ui->pushDownload->setEnabled(old);
    ui->pushNotes->setEnabled(old);
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
    settings.setValue("io/autoSave", value);
    settings.setValue("online/useautoupload", ui->checkAutoUpload->isChecked());
}

void PreferencesDialog::on_pushNotes_clicked()
{
    QMessageBox::information(nullptr,
                             tr("Über Version %1").arg(online.toString()),
                             CoreApplication::loadNotes(online),
                             QMessageBox::Ok, QMessageBox::Ok);
}
