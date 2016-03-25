#include "planerfahrtage.h"
#include "ui_planerfahrtage.h"
#include <QMessageBox>
#include <QFileDialog>
#include "export.h"
#include "fahrtag.h"
#include <QListWidgetItem>
#include <QDate>
#include <QTime>


void PlanerFahrtage::on_ButtonAdd_clicked () {
    QListWidgetItem *neuesFeld = new QListWidgetItem("Test");
    Fahrtag *neuerFahrtag = new Fahrtag(neuesFeld, ui->listRes);

    QVariant v = qVariantFromValue((void *) neuerFahrtag);
    neuesFeld->setData(Qt::UserRole, v);

    ui->ListeZuege->addItem(neuesFeld);
    fahrplanManager->addFahrtag(neuerFahrtag);
    ui->ButtonRemove->setEnabled(true);
    saved = false;
    aktualisieren();
}
void PlanerFahrtage::on_ButtonRemove_clicked() {
    if (QMessageBox::question(this, "Bitte bestätigen!", "Möchten Sie das wirklich löschen?") == QMessageBox::Yes) {
        fahrplanManager->removeFahrtag(fahrplanManager->getFahrtag(aktuellerZug));
        saved = false;
    }
    if (ui->ListeZuege->count() < 1)
        ui->ButtonRemove->setEnabled(false);

}

void PlanerFahrtage::on_ListeZuege_itemSelectionChanged()
{
    if (ui->ListeZuege->count() > 0)
        on_ListeZuege_itemClicked(ui->ListeZuege->currentItem());
}

void PlanerFahrtage::on_ListeZuege_itemClicked(QListWidgetItem *item)
{
    uebernehmen = false;
    setState(true);
    ZugSichern();
    aktuellerZug = NULL;
    if (item != NULL)
        ZugLaden(item);
    uebernehmen = true;

}

void PlanerFahrtage::on_calendarDock_clicked(const QDate &date) {
    fahrplanManager->showDate(date);
}
void PlanerFahrtage::on_pushListeShowAll_clicked() {
    fahrplanManager->showAll();
}


/*  Elemente zu Liste hinzufügen und löschen */
// Fahrplan

// Personal
void PlanerFahrtage::on_toolButtonListTfDelete_clicked() { deleteSelectedRowFromListWidget(ui->listTf); }
void PlanerFahrtage::on_toolButtonListTfAdd_clicked() { addEmptyRowToListWidget(ui->listTf); }
void PlanerFahrtage::on_toolButtonListZfDelete_clicked() { deleteSelectedRowFromListWidget(ui->listZf); }
void PlanerFahrtage::on_toolButtonListZfAdd_clicked() { addEmptyRowToListWidget(ui->listZf); }
void PlanerFahrtage::on_toolButtonListZubDelete_clicked() { deleteSelectedRowFromListWidget(ui->listZub); }
void PlanerFahrtage::on_toolButtonListZubAdd_clicked() { addEmptyRowToListWidget(ui->listZub); }
void PlanerFahrtage::on_toolButtonListServiceDelete_clicked() { deleteSelectedRowFromListWidget(ui->listService); }
void PlanerFahrtage::on_toolButtonListServiceAdd_clicked() { addEmptyRowToListWidget(ui->listService); }


/* Sichern der Daten, wenn Eingaben verändert wurden */
// Organisatorisches
void PlanerFahrtage::on_dateZug_dateChanged(const QDate &date) { saveRahmendaten(); }
void PlanerFahrtage::on_comboArt_currentIndexChanged(int index) { saveRahmendaten(); }
void PlanerFahrtage::on_textAnlass_textChanged() { saveRahmendaten(); }
void PlanerFahrtage::on_checkWichtig_stateChanged(int arg1) { saveRahmendaten(); }
void PlanerFahrtage::on_comboWagenreihung_currentTextChanged(QString string) { saveRahmendaten(); }
void PlanerFahrtage::on_comboTimeTfH_currentIndexChanged(int index) { saveRahmendaten(); }
void PlanerFahrtage::on_comboTimeTfM_currentTextChanged(QString string) { saveRahmendaten(); }
void PlanerFahrtage::on_comboTimeZH_currentIndexChanged(int index) { saveRahmendaten(); }
void PlanerFahrtage::on_comboTimeZM_currentTextChanged(QString string) { saveRahmendaten(); }

// Fahrplan

// Personal
void PlanerFahrtage::on_listTf_itemChanged(QListWidgetItem *item) { savePersonal(); }
void PlanerFahrtage::on_listZf_itemChanged(QListWidgetItem *item) { savePersonal(); }
void PlanerFahrtage::on_listZub_itemChanged(QListWidgetItem *item) { savePersonal(); }
void PlanerFahrtage::on_listService_itemChanged(QListWidgetItem *item) { savePersonal(); }
