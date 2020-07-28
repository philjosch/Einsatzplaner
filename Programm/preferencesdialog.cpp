#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include "coreapplication.h"
#include "einstellungen.h"

#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>

PreferencesDialog::PreferencesDialog(QWidget *parent) : QDialog(parent), ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);
    ui->buttonGroupSortierung->setId(ui->radioVorNach, 1);
    ui->buttonGroupSortierung->setId(ui->radioNachVor, 0);
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(closeDialogOk()));
    connect(ui->pushDownload, &QPushButton::clicked, this, [=]() { QDesktopServices::openUrl(CoreApplication::URL_DOWNLOAD); });
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

void PreferencesDialog::on_pushNotes_clicked()
{
    QMessageBox::information(nullptr,
                             tr("Über Version %1").arg(online.toString()),
                             CoreApplication::loadNotes(online),
                             QMessageBox::Ok, QMessageBox::Ok);
}
