#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include "coreapplication.h"

#include <QDesktopServices>
#include <QSettings>
#include <QUrl>

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
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
    QString aktuell = QCoreApplication::applicationVersion();
    QString online = CoreApplication::loadVersion();
    bool old = CoreApplication::versionGreater(online, aktuell);
    QString s = tr("Fehler bei der Anfrage");
    if (old) {
        s = tr("Sie verwenden Version %1. Es ist Version %2 verfügbar.").arg(aktuell, online);
    } else {
        s =  tr("Sie verwenden bereits die neuste Version %1.").arg(aktuell);
        // Version auf dem aktuellsten Stand
    }
    ui->labelVersion->setText(s);
    ui->pushDownload->setEnabled(old);
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
