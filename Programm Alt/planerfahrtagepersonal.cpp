#include "ui_planerfahrtage.h"
#include "planerfahrtage.h"
#include "fahrtag.h"

void PlanerFahrtage::savePersonal()
{
    if (uebernehmen)
    {
        // Liste mit Peronal Speichern
        aktuellerZug->setBenoetigeTf(ui->checkTf->isChecked());
        aktuellerZug->setBenoetigeZf(ui->checkZf->isChecked());
        aktuellerZug->setBenoetigeZub(ui->checkZub->isChecked());
        aktuellerZug->setBenoetigeService(ui->checkService->isChecked());
        aktuellerZug->setTf(getQListFromListWidget(ui->listTf));
        aktuellerZug->setZf(getQListFromListWidget(ui->listZf));
        aktuellerZug->setZub(getQListFromListWidget(ui->listZub));
        aktuellerZug->setService(getQListFromListWidget(ui->listService));
        aktuellerZug->setBemerkungen(ui->textBemerkungen->toPlainText());
        setSaved(false);
    }
}

void PlanerFahrtage::on_pushButtonListTfDelete_clicked()
{
    deleteSelectedRowFromListWidget(ui->listTf);
    if (ui->listTf->count() == 0) ui->pushButtonListTfDelete->setEnabled(false);
}
void PlanerFahrtage::on_pushButtonListZfDelete_clicked()
{
    deleteSelectedRowFromListWidget(ui->listZf);
    if (ui->listZf->count() == 0) ui->pushButtonListZfDelete->setEnabled(false);
}
void PlanerFahrtage::on_pushButtonListZubDelete_clicked()
{
    deleteSelectedRowFromListWidget(ui->listZub);
    if (ui->listZub->count() == 0) ui->pushButtonListZubDelete->setEnabled(false);
}
void PlanerFahrtage::on_pushButtonListServiceDelete_clicked()
{
    deleteSelectedRowFromListWidget(ui->listService);
    if (ui->listService->count() == 0) ui->pushButtonListServiceDelete->setEnabled(false);
}

void PlanerFahrtage::on_pushButtonListTfAdd_clicked()
{
    addEmptyRowToListWidget(ui->listTf);
    ui->pushButtonListTfDelete->setEnabled(true);
}
void PlanerFahrtage::on_pushButtonListZfAdd_clicked()
{
    addEmptyRowToListWidget(ui->listZf);
    ui->pushButtonListZfDelete->setEnabled(true);
}
void PlanerFahrtage::on_pushButtonListZubAdd_clicked()
{
    addEmptyRowToListWidget(ui->listZub);
    ui->pushButtonListZubDelete->setEnabled(true);
}
void PlanerFahrtage::on_pushButtonListServiceAdd_clicked()
{
    addEmptyRowToListWidget(ui->listService);
    ui->pushButtonListServiceDelete->setEnabled(true);
}

void PlanerFahrtage::on_checkTf_stateChanged(int arg1) { savePersonal(); }
void PlanerFahrtage::on_checkZf_stateChanged(int arg1) { savePersonal(); }
void PlanerFahrtage::on_checkZub_stateChanged(int arg1) { savePersonal(); }
void PlanerFahrtage::on_checkService_stateChanged(int arg1) { savePersonal(); }

void PlanerFahrtage::on_listTf_itemChanged(QListWidgetItem *item) { savePersonal(); }
void PlanerFahrtage::on_listZf_itemChanged(QListWidgetItem *item) { savePersonal(); }
void PlanerFahrtage::on_listZub_itemChanged(QListWidgetItem *item) { savePersonal(); }
void PlanerFahrtage::on_listService_itemChanged(QListWidgetItem *item) { savePersonal(); }
