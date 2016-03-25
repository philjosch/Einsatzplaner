#include "planerfahrtage.h"
#include "ui_planerfahrtage.h"
#include <QMessageBox>
#include <QFileDialog>
#include "export.h"
#include "qfahrtag.h"
#include <QListWidgetItem>
#include <QDate>
#include <QTime>


/* Allgemeine Aktionen */
// Liste mit Reservierungen verändern
void PlanerFahrtage::on_pushResAllAuto_clicked() {

}

void PlanerFahrtage::on_pushResVerteilung_clicked() {

}

// Manipulieren der Liste der Reservierungen
void PlanerFahrtage::on_toolResAdd_clicked() { fahrplanManager->getFahrtag(aktuellerZug)->getManager()->add(); }
void PlanerFahrtage::on_toolResDelete_clicked() {
    QListWidgetItem *item = ui->listRes->currentItem();
    fahrplanManager->getFahrtag(aktuellerZug)->getManager()->remove(*item);
}
/* Aktionen */
// Aktionen, wenn die Radio-Buttons geklickt werden, kommt aber erst in einer weiteren Version


/* Änderungen bei der Eingabe direkt speichern */
void PlanerFahrtage::on_lineResName_textChanged(const QString &arg1) { saveReservierungen(); }
void PlanerFahrtage::on_spinResAnzahl_valueChanged(int arg1) { saveReservierungen(); }
void PlanerFahrtage::on_comboResKlasse_currentIndexChanged(int index) { saveReservierungen(); }
void PlanerFahrtage::on_checkResFahrrad_stateChanged(int arg1) { saveReservierungen(); }
void PlanerFahrtage::on_lineResMail_textChanged(const QString &arg1) { saveReservierungen(); }
void PlanerFahrtage::on_lineResTelefon_textChanged(const QString &arg1) { saveReservierungen(); }
void PlanerFahrtage::on_comboResStart1Zug_currentIndexChanged(int index) { saveReservierungen(); }
void PlanerFahrtage::on_comboResStart1Hp_currentIndexChanged(int index) { saveReservierungen(); }
void PlanerFahrtage::on_comboResEnde1Zug_currentIndexChanged(int index) { saveReservierungen(); }
void PlanerFahrtage::on_comboResEnde1Hp_currentIndexChanged(int index) { saveReservierungen(); }
void PlanerFahrtage::on_radioResAuto_clicked(bool checked) { saveReservierungen(); }
void PlanerFahrtage::on_radioResManu_clicked(bool checked) { saveReservierungen(); }
void PlanerFahrtage::on_lineResSitze_textChanged(const QString &arg1) { saveReservierungen(); }
void PlanerFahrtage::on_plainResSonstiges_textChanged() { saveReservierungen(); }

